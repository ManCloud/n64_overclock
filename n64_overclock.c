/* -----------------------------------------------------------------------
*N64 'Overclock-Mod Multiplier Selector'
*
* Copyright (C) 2018 by Christian Ruecker <mancloud20@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 of the License only.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* -----------------------------------------------------------------------
*
*   This program is designed to run on a ATTINY13/V microcontroller connected
*   to the PIF-NUS (Lockoutchip) of the N64. It allows you to select the
*   multiplier of the CPU via the reset button.
*
*   pin configuration: [PIF NUS / CPU]
*                              ,------_-----.
*    Reset in [PIF-NUS Pad 27] |1  PB5     8| Vcc/3.3V [PIF-NUS Pin 28]
*              LED-color 0 out |2  PB3 PB2 7| multiplier 0 out [CPU Pin 112]
*              LED-color 1 out |3  PB4 PB1 6| multiplier 1 out [CPU Pin 116]
*     Vss/GND [PIF-NUS Pin 14] |4      PB0 5| Reset out [PIF-NUS Pin 27]
*                              `------------'
*
*   As the internal oscillator is used, you should connect a capacitor of about 100nF between
*   Pin 1 (Vcc/+3.3V) and Pin 8 (Vss/GND) as close as possible to the PIC. This esures best
*   operation.
*
*   multiplier and their corresponding operating frequencies:
*
*        CPU Pin    |            |
*       Assignment  | Multiplier |  Operating
*      116  |  112  |            |  Frequency
*    -------|-------|------------|--------------
*      GND  |  GND  |    1.0x    |   62.50 MHz   (not implemented here)
*      GND  | +3.3V |    1.5x    |   93.75 MHz   (default)
*     +3.3V |  GND  |    2.0x    |  125.00 MHz
*     +3.3V | +3.3V |    3.0x    |  187.50 MHz
*
*
*   This program expects a dual-LED with a common cathod. Further, mode '0',
*   i.e., 62.50 MHz is not allowed. If you want to have the same program using
*   a LED with a common anode or doesn't skip mode '0', please change it on
*   your own or contact me.
*
* -----------------------------------------------------------------------*/

#include "n64_overclock.h"

int main(void)
{
	resetOut_assign();		//set console to reset -> do this as early as possible

	uint16_t delayTicks = 0;
	multiplier_e currentMultiplier = MINIMUM_MULTIPLIER;
	bool multiplierLocked = false;
	machineState_t currState = STATE_INIT;

	while(1)
	{
		switch(currState)
		{
			case STATE_INIT:
			{
				ACSR |= (1<<ACD);    	//Analogcomparator OFF
				mult0_init();
				mult1_init();
				led0_init();
				led1_init();
				resetIn_init();
				timer_init();
				set_sleep_mode(SLEEP_MODE_IDLE);
				sleep_enable();
				sei();	// enable global interrupts
				currentMultiplier = eeprom_read_byte(CONFIG_ADDRESS);
				currState = STATE_UPDATE_MULTIPLIER;
			}break;
			case STATE_UPDATE_MULTIPLIER:
			{
				if(currentMultiplier > MAXIMUM_MULTIPLIER)
				{
					currentMultiplier = MINIMUM_MULTIPLIER;
				}

				if(currentMultiplier == MULTIPLIER_1_5X || currentMultiplier == MULTIPLIER_3_0X)
				{
					if(!multiplierLocked) mult0_high();
					led0_on();
				}
				else
				{
					if(!multiplierLocked) mult0_low();
					led0_off();
				}

				if(currentMultiplier == MULTIPLIER_2_0X || currentMultiplier == MULTIPLIER_3_0X)
				{
					if (!multiplierLocked) mult1_high();
					led1_on();
				}
				else
				{
					if(!multiplierLocked) mult1_low();
					led1_off();
				}

				if(multiplierLocked)	//multiplier is locked ==> next state is nextMode
				{
					delayTicks = TIMER_DELAY_MODE;
					currState = STATE_NEXT_MODE;
				}
				else
				{
					eeprom_update_byte(CONFIG_ADDRESS, currentMultiplier);
					resetOut_release();
					multiplierLocked = true;
					currState = STATE_IDLE;
				}
			}break;
			case STATE_IDLE:
			{
				if(resetIn_isAssigned())
				{
					delayTicks = TIMER_DEBOUNCE_TICKS;
					currState = STATE_DEBOUNCE;
				}
			}break;
			case STATE_DEBOUNCE:
			{
				if(delayTicks)
				{
					delayTicks--;
				}
				else
				{
					if(resetIn_isAssigned())
					{
						delayTicks = TIMER_DELAY_MODE;
						currState = STATE_CHECK4RESET;
					}
					else
					{
						currState = STATE_IDLE;
					}
				}
			}
			break;
			case STATE_CHECK4RESET:
			{
				if(resetIn_isAssigned())
				{
					if(delayTicks)
					{
						delayTicks--;
					}
					else
					{
						currState = STATE_NEXT_MODE;
					}
				}
				else
				{
					resetOut_assign();
					delayTicks = TIMER_DELAY_300MS;
					currState = STATE_DO_RESET;
				}
			}
			break;
			case STATE_NEXT_MODE:
			{
				if(resetIn_isAssigned())
				{
					if(delayTicks)
					{
						delayTicks--;
					}
					else
					{
						currentMultiplier++;
						currState = STATE_UPDATE_MULTIPLIER;
					}
				}
				else
				{
					eeprom_update_byte(CONFIG_ADDRESS, currentMultiplier);
					currState = STATE_IDLE;
				}

			}break;
			case STATE_DO_RESET:
			{
				if(delayTicks)
				{
					delayTicks--;
				}
				else
				{
					resetOut_release();
					currState = STATE_IDLE;
				}
			}
			break;
			default: //illegal state: shall never happen
				break;
		}
	sleep_cpu();
	}
}


/*
 * Der Overflow Interrupt Handler
 * wird aufgerufen, wenn TCNT0 von
 * 255 auf 0 wechselt (256 Schritte),
 * d.h. ca. alle 2 ms
 */
ISR (TIMER_VECT)
{
	/* Interrupt Aktion alle
	 * (128000)/256 Hz = 500
	 * bzw.
	 * 1/500 s = 2 ms
	 */
}
