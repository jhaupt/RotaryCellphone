# RotaryCellphone
## Firmware boards without the FONA Tx pin re-wire modification

This is Justine's 19 April 2020 firmware with the following fixes:

* Tests if the FONA baud rate is 4800, 9600 or 115200 baud, and sets 4800 baud in NVM if necessary.
* Sets the receive pin for FONA data back to input after a display write (see *hack* below).
* Fixes no tone for dialled zero.

The FONA 3G is known for difficult auto baud rate detection (or at least not well documented). The baud rate wasn't getting set correctly, resulting in the *no dial tone*, or *unable to answer calls* bugs. The FONA module doesn't need to communicate with the ATMEGA to ring but it needs communication to answer a call or send tones through the speaker.

An alternative method to set baud rate in Non-Volatile Memory (NVM) is to connect a serial via USB terminal (115200, 8-N-1) directly to the FONA USB socket and issue the command:

    AT+IPREX=4800

Hack: each use of the SPI bus to write to the display causes pin D53 to switch to output mode. To switch back to input (to receive FONA data) this line is appended to every display update:

    pinMode(Rx, INPUT_PULLUP);

This is a *'rough & ready hack'* because the ATmega will crash if the display updates while the FONA Tx pin sends data. The pin used to receive FONA data (Arduino D53) is shared with the SPI SS pin which means during display writes, an output is connected to an output.

The display only updates while recalling names/numbers from memory therefore this crash should be a rare occurrence.

#### Further info

Search simcom.com for: *UART + Application + Notes* (registration and login required), look for a 'synchronize baud rate'.

| Title | Notes |
|-------|-------|
| sim52xx_uart_application_note_v0.01.pdf | no baud rate sychronisation info |
| SIM5300E_UART_Application_Note_V1.01.pdf | possibly most relevant; desc & flowchart
| SIM5360_UART_Application_Note_V0.05.pdf | description & flowchart but wrong list of baud rates |
| SIM7X00 Series_UART_Application Note_V1.00.pdf | description & flowchart |

Some notes state send 'at' for synchronisation, the SIM5300E note states send 'AT', both seem to work.

SIM5320X supported baud rates (SIMCOM_SIM5320_ATC_EN_V2.05.pdf):  
300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 
3200000, 3686400, 4000000  
but the range of supported auto baud rates may be less.

Why send 'AT' for synchronisation? See [here.](https://en.wikipedia.org/wiki/Hayes_command_set#Autobaud "Wikipedia on 'automatic baud rate detection'")
