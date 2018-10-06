/*
 * n64_overclock.h
 *
 *  Created on: 03.10.2018
 *      Author: ManCloud
 */

#ifndef N64_OVERCLOCK_H_
#define N64_OVERCLOCK_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/sleep.h>

#define COMMON_CATHODE 0
#define COMMON_ANODE 1
//#define LED_TYPE COMMON_CATHODE 		//0 == common cathode; 1 == common anode

#define MULT_0_PORT 	PORTB
#define MULT_0_DIR 		DDRB
#define MULT_0_PIN 		PIN2
#define mult0_init() 	MULT_0_DIR |= (1 << MULT_0_PIN)
#define mult0_low()		MULT_0_PORT &= ~(1 << MULT_0_PIN)
#define mult0_high()	MULT_0_PORT |= (1 << MULT_0_PIN)

#define MULT_1_PORT 	PORTB
#define MULT_1_DIR 		DDRB
#define MULT_1_PIN 		PIN1
#define mult1_init() 	MULT_1_DIR |= (1 << MULT_1_PIN)
#define mult1_low()		MULT_1_PORT &= ~(1 << MULT_1_PIN)
#define mult1_high()	MULT_1_PORT |= (1 << MULT_1_PIN)

#define RESET_OUT_PORT 		PORTB
#define RESET_OUT_DIR 		DDRB
#define RESET_OUT_PIN 		PIN0
#define resetOut_assign() 	RESET_OUT_PORT &= ~(1 << RESET_OUT_PIN); RESET_OUT_DIR |= (1 << RESET_OUT_PIN)
#define resetOut_release()	RESET_OUT_DIR &= ~(1 << RESET_OUT_PIN); RESET_OUT_PORT |= (1 << RESET_OUT_PIN)

#define RESET_IN_PORT 	PORTB
#define RESET_IN_DIR 	DDRB
#define RESET_IN_INPUT	PINB
#define RESET_IN_PIN 	PIN5
#define resetIn_init()	RESET_IN_DIR &= ~(1 << RESET_IN_PIN)
#define resetIn_isAssigned() ((RESET_IN_INPUT & (1 << RESET_IN_PIN)) == 0)

#define LED_0_PORT 		PORTB
#define LED_0_DIR 		DDRB
#define LED_0_PIN 		PIN3
#define led0_init() 	LED_0_DIR |= (1 << LED_0_PIN)
#if LED_TYPE
#define led0_on() 		LED_0_PORT &= ~(1 << LED_0_PIN);
#define led0_off() 		LED_0_PORT |= (1 << LED_0_PIN)
//#define LED_0_TOGGLE 	if(LED_0_PORT & (1 << LED_0_PIN)){LED_0_ON;}else{LED_0_OFF;}
#else
#define led0_on() 		LED_0_PORT |= (1 << LED_0_PIN)
#define led0_off() 		LED_0_PORT &= ~(1 << LED_0_PIN)
//#define LED_0_TOGGLE 	if(LED_0_PORT & (1 << LED_0_PIN)){LED_0_OFF;}else{LED_0_ON;}
#endif
#define led0_toggle() 	LED_0_PORT ^= (1 << LED_0_PIN)

#define LED_1_PORT 		PORTB
#define LED_1_DIR 		DDRB
#define LED_1_PIN 		PIN4
#define led1_init() 	LED_1_DIR |= (1 << LED_1_PIN)
#if LED_TYPE
#define led1_on() 		LED_1_PORT &= ~(1 << LED_1_PIN);
#define led1_off() 		LED_1_PORT |= (1 << LED_1_PIN)
#else
#define led1_on() 		LED_1_PORT |= (1 << LED_1_PIN)
#define led1_off() 		LED_1_PORT &= ~(1 << LED_1_PIN)
#endif
#define led1_toggle() 	LED_1_PORT ^= (1 << LED_1_PIN)

typedef enum machineState_tag
{
	STATE_INIT,
	STATE_UPDATE_MULTIPLIER,
	STATE_IDLE,
	STATE_DEBOUNCE,
	STATE_CHECK4RESET,
	STATE_NEXT_MODE,
	STATE_SAVE_MODE,
	STATE_DO_RESET,
	STATE_FINISH
}machineState_t;

typedef enum multiplier_enum
{
	MULTIPLIER_1_0X,
	MULTIPLIER_1_5X,
	MULTIPLIER_2_0X,
	MULTIPLIER_3_0X
}multiplier_e;


#define MINIMUM_MULTIPLIER MULTIPLIER_1_5X
#ifndef MAXIMUM_MULTIPLIER
	#define MAXIMUM_MULTIPLIER MULTIPLIER_2_0X
#endif
#define TIMER_PRESCALER 		(1<<CS00)	//prescaler 1
#define TIMER_DEBOUNCE_TICKS  	0x000A
#define TIMER_DELAY_100MS 		0x0032
#define TIMER_DELAY_200MS 		0x0064
#define TIMER_DELAY_300MS 		0x0096
#define TIMER_DELAY_MODE  		0x01FF	//~1,020 s

#define CONFIG_ADDRESS 	((void *) 0x00)
#if defined (__AVR_ATtiny2313__)
#define TIMER_VECT TIMER0_OVF_vect
#define timer_init() TCCR0B = TIMER_PRESCALER; TIMSK |= (1<<TOIE0);		// Prescaler 256 + enable Timer0 overflow interrupt
#elif defined (__AVR_ATtiny13__)
#define TIMER_VECT TIM0_OVF_vect
#define timer_init() TCCR0B = TIMER_PRESCALER; TIMSK0 |= (1<<TOIE0);		// Prescaler 256 + enable Timer0 overflow interrupt
#elif defined (__AVR_ATtiny45__)
#define TIMER_VECT TIM0_OVF_vect
#define timer_init() TCCR0B = TIMER_PRESCALER; TIMSK |= (1<<TOIE0);		// Prescaler 256 + enable Timer0 overflow interrupt
#endif


#endif /* N64_OVERCLOCK_H_ */
