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
Changed name of *main.ino* to *RotaryCellphone.ino*. This removes requirement to put everything in a *main* subfolder and makes development easier. All sketch files should now be directly under the folder: *RotaryCellphone* .

Hardware mod required: change the FONA Tx pin connection from ATmega pin 19 to pin 25 (aka Arduino digital 12). This fixes the contention issue between FONA Tx and the SPI SS pin. See the forum thread "Firmware stuff" for details and photo [SteveC].
