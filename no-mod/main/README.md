# RotaryCellphone
## Firmware boards without the FONA Tx pin re-wire modification

This is Justine's 19 April 2020 firmware with the following fixes:

* Tries to set FONA baud rate to 4800, once accepted this is stored in non-volatile memory.
* Sets the receive pin for FONA data back to input after a display update (hack - see below).
* Fixes no tone for dialled zero.

The FONA 3G is known for auto baud rate detection being unreliable therefore the baud rate wasn't getting set correctly, resulting in the 'no dial tone' bug.

An alternative method to set baud rate (non volatile memory) is to connect a serial via USB terminal (115200, 8-N-1) directly to the FONA USB socket and issue the command:

    AT+IPREX=4800

Hack: each use of the SPI bus to write to the display causes pin D53 to switch to output mode. To switch back to input (to receive FONA data) this line is appended to every display update:

    pinMode(Rx, INPUT_PULLUP);

This is a *'rough & ready hack'* because the ATmega will crash if the display updates while the FONA Tx pin sends data. The pin used to receive FONA data (Arduino D53) is shared with the SPI SS pin which means during display writes, an output is connected to an output.

Fortunately the display only updates while recalling names/numbers from memory therefore this should be a rare occurrence.
