# RotaryCellphone
## Firmware boards without the FONA Tx pin re-wire modification

This is Justine's 19 April 2020 firmware with the following fixes:

* Sets FONA baud rate to 4800 (non-volatile) without relying on auto baud rate detection (fixes no dial tones).
* Sets the receive pin for FONA data back to input after a display update (hack).
* Fixes no tone for dialled zero.

This is a hack because the ATmega will crash if the display updates while the FONA Tx pin sends data. Fortunately the display only updates while recalling names/numbers from memory.
