# RotaryCellphone
The motive behind this fork is to get the original firmware (April 2020), to a fully functional base. It is not the intention to add a host of new functions, localisation or customisation but simply to get the original framework to fully functional state.

There is one new function; what phone doesn't tell the time? An analog clock would be appropriate but for now it's a simple 2 line digital date/time display.

Incoming caller ID is working. The display will show either date/time or caller ID depending on the postion of the mode switch. They show at the bottom of the display using fast partial updates. Suggestion: it's possible the caller ID message will include a name if the number is found in the FONA/Sim address book. If so the caller ID could easily show a name. See the AT+CLIP section of the SIMCom AT Command Set manual for more information.

There's a new welcome display with logo to show how easily bitmaps can be displayed. Perhaps leading to functional icons.

I don't intend to add much more... otherwise I would buy a smartphone! I welcome reports of bugs/issues then I'll likely archive this after a few months. There are too many possibilities for localisation and customisation and I don't want to get too tied up with this. After all this is a DIY project, have fun with customisations, make it personal.

Enjoy, it's been fun building this in lockdown!

Thanks Justine.

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

- Display time from cell network - *done*

- Parse and display incoming call ID - *done*

- Get NP mode to dial variable length numbers - *maybe*...
