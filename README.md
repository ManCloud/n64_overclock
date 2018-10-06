# n64_overclock
Switchless Overclocking Mod for N64

Atmel port of [Botis4938s N64 Overclocking Mod: Switchless with PIC12F629](https://circuit-board.de/forum/index.php/Thread/15739-Nintendo-64-Overclock-schalterlos-mit-PIC12F629/) [(github)](https://github.com/borti4938/Switchless-Mods/tree/master/N64/Overclock) 

## Function
- The µC will initially start with the minimum multiplier (default: 1.5x)
- shortly press the reset Button: reset console
- press and hold reset button: LED switches colors to indicate the modes (depending on your configuration).
  - order is (without 1.0x): 1.5x -> 2.0x (-> 3.0x) -> 1.5x -> and so on...
- in order to save the current mode you need to powercycle the console! A simple reset is not enough casue a mode-change during console operation would lead to crash.

## How-To build

- To build the project you need
  - the sources (obviously)
  - a c-compiler for AVR (WinAVR, Atmel-Toolchain, ATmel-Studio or else...)
- Then you'll need to decide which controller you want to use
  - tested controllers: ATtiny13, ATtiny45, ATtiny2313
  - others of this family should also work but are not implemented!
- build syntax:
  - the build command is: make {controller}_[{maxMultiplier}]_{LedType}
    - example 1: (MCU: ATtiny13, Max. Multiplier: 3.0x, Led-Type: Common Anode)
      - make tiny13_3_0_CA
    - example 2: (MCU: ATtiny45, Max. Multiplier: 2.0x, Led-Type: Common Cathode)
      - make tiny45_CC

## How-To flash
- To flash this project you need
  - a µController (obviously)
  - a ISP-Flasher (USBTinyISP, STK500, etc...)
  - a Flasher-Tool (Atmel-Studio, avrdude, etc...)
- After building the project you will have a .hex file named like "n64_overclock_t13_3_0x_CA"
- flash the hex file with your Flasher-Tool
- after that we have to set the fusebits:
  - cause of lack of pins on the 8-Legged devices, we need to disable the reset button.
  - **warining**: when you disabled the reset button, the only possible way of flashing/erasing the µController is via HVPP/HVSP (High Voltage Programming)
  - Fuses for:
    - ATtiny13:   low:**0x7B** high:**0xFE**
    - ATtiny45:   low:**0xE4** high:**0x5F**
    - ATtiny2313: low:**0xE6** high:**0xDF**
## How-To connect
- To connect this Mod you need:
  - a N64 (obviously)
  - a flashed µController
  - a 100nF ceramic capacitor
  - a Multicolor LED (according to your flashed Firmware)
  - two 100 Ohm resistors (connected to the appropriate pin)
  - a Gambit Screwdriver
- Pinout:

| µController-Pin   | Description  | Target                            |
| ----------------- | -----------  | --------------------------------- |
| Pin 1             | Reset In     | PIF-NUS Pad 27 / Reset Switch Pad |
| Pin 2             | LED-color 0  | Led 0 / LED-Leg 1                 |
| Pin 3             | Led-color 1  | Led 1 / LED-Leg 2                 |
| Pin 4             | Vss/GND      | PIF-NUS Pin 14                    |
| Pin 5             | Reset Out    | PIF-NUS Pin 27                    |
| Pin 6             | multiplier 0 | CPU Pin 112                       |
| Pin 7             | multiplier 1 | CPU Pin 116                       |
| Pin 8             | Vcc/+3V3     | PIF-NUS Pin 28                    |

- For further instructions go [(here)](https://circuit-board.de/forum/index.php/Thread/15739-Nintendo-64-Overclock-schalterlos-mit-PIC12F629/) or [(here)](https://circuit-board.de/forum/index.php/Thread/5657-Nintendo-64-Overclock-L%C3%BCfter-Mod/) (german links)
