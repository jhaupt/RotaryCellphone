# RotaryCellphone
Firmware for the Open Source Rotary Cellphone

Regarding references to "ModeSwitch_631". The mode switch is a SP3T switch which changes the operating mode of the phone. The primary mode, as listed in this code,is "631", which is my area code. In this mode, 631 is prepended automatically to any 7 digit numbers dialed. All instances of this must be changed to your preferred area code. The other two modes are "NP" and "Alt". NP stands for "no prepend", to enter full 10-digit numbers, and "Alt" is the alternate mode. To minimize the total number of switches, the function of the bottom four buttons on the phone changes depending on the position of the mode switch. 

Arduino IDE needs board support for 2560V. For this, add "MegaCore" board configuration from here: https://github.com/MCUdude/MegaCore

Also add the Adafruit EOD and the GxEPD2 libraries, which can be added from within the Arduino IDE by going to tools > Manage Libraries...

Use an AVR-ISP-MK2 to flash firmware. This is much easier than dealing with the USB port for programming. Just plug the programmer into the ICSP header and use "Ctrl+Shift+U" within the Arduino IDE to compile and upload. No need to pick a dev assignment for the USB port.

The board settings under Tools in the Arduino IDE should be as follows (after installing the above MegaCore thingy):
Board: ATmega2560
Clock: External 8MHz
BOD: BOD 2.7v
Compiler LTO: LTO enabled
Pinout: Arduino MEGA pinout
Bootloader: Yes (UART0)

## Issues & Troubleshooting
### Hardware mod
FONA Tx cannot talk to the ATmega2560 microcontroller after the e-paper display is initialised. This is because the FONA tries to talk to the ATmega on pin 19, which is also the ATmega hardware SPI SS pin. When the ATmega is configured as master, SS is configured as output with idle state high.

In theory the SS pin could be switched back to input after display initialisation but when the FONA pulls SS low, the ATmega SPI switches to slave mode (see datasheet section 21.1.2). The display then stops working. This mode switch is done in ATmega hardware, it can't be changed.

The best solution is to use another pin for FONA Tx. **This fork requires that FONA Tx is re-wired from pin 19 to pin 25**. The later was chosen because of its proximity, corner pins are easier to solder to and pin 25, (aka Aduino digital pin 12) is one of the few Mega pins that work with Software Serial.

![Preparing the via](images/hwmod_part1.jpg "Cut the track and prepare the via as a solder pad")

![Re-wire FONA Tx](images/hwmod_part2.jpg "Re-wiring FONA Tx with 0.2mm 'Vero' wire")

FONA Tx rewired to ATmega pin 25 with 0.2mm 'Vero' wire.

### Filename change
Changed name of *main.ino* to *RotaryCellphone.ino*. This removes requirement to put everything in a *main* subfolder and makes development a little easier from a Git local repository. All sketch files should be directly under the folder: *RotaryCellphone*.

## Roadmap
This fork will require the FONA Tx hardware re-wire mod. The aim is to follow the original approach as closly as possible and keep the rotary dial routines.

It might be possible to use a third party phone module library, some have been considered. TinyGSM is compatible with the SIM5320 but currently not for voice - keep an eye on TinyGSM developments. The Adafruit FONA library requires the RI (ring indicator) wired to a pin using a specific interupt (another hardware mod would be required).

This fork will not attempt localisation (nor localization)! There's a big world outside North America's 10 digit numbering plan and the International Telecommunication Union (ITU) allows up to 15 digits. As Justine states: "This is a DIY project", any necessary localisation or customisation is left to the DIYer. [Telephone numbering plan](https://en.wikipedia.org/wiki/Telephone_numbering_plan "Telephone numbering plan"): many countries have variable length area codes.

- Reliably decode FONA messages by parsing AT command responses - *done*

- Fix Signal strength and battery charge display - *done*

- Display time from cell network - *in progress*

- Parse and display incoming call ID... *in progress*...
