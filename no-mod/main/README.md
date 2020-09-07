# RotaryCellphone
## Firmware boards without the FONA Tx pin re-wire modification

This is Justine's 19 April 2020 firmware with the following fixes:

* Sets FONA baud rate to 4800 (non-volatile) without relying on auto baud rate detection.
* Sets the receive pin for FONA data back to input after a display update (hack - see below).
* Fixes no tone for dialled zero.

The FONA 3G is known for auto baud rate detection being unreliable therefore the baud rate wasn't getting set correctly, resulting in the 'no dial tone' bug.

An alternative method to set baud rate (non volatile memory) is to connect a serial via USB terminal (115200, 8-N-1) directly to the FONA USB socket and issue the command:

    AT+IPREX=4800

Hack: because the pin used to receive FONA data (Arduino D53) is shared with the SPI SS pin, the ATmega will crash if the display updates while the FONA Tx pin sends data. Fortunately the display only updates while recalling names/numbers from memory therefore this should be a rare occurrence.
