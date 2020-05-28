/***************************************************
* MIT License
*
* Copyright (c) 2020 Justine Haupt et al
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
****************************************************
Rotary cellphone original firmware written by Justine Haupt. 
MIT license, all text above must be included in any redistribution.

1/12/2020: v1.0
1/13/2020: v1.1
1/18/2020: v1.1.1

2020/05/26: FONA receive fixes (require hardware mod) - SteveC

  Cellphone program for ATMega2560-based board controlling the Adafruit FONA v1. Adafruit libraries are not used for the FONA
  but this does use the Adafruit 2.13" Tri-Color E-Ink display with the associated libraries. The license from Lady Ada for
  that part of the program is as follows:

    ***************************************************
    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!
    Written by Limor Fried/Ladyada for Adafruit Industries.
    MIT license, all text above must be included in any redistribution
   ****************************************************

NOTES:
Regarding references to "ModeSwitch_631". The mode switch is a SP3T switch which changes the operating mode of the phone. The primary mode, as listed in this code,
is "631", which is my area code. In this mode, 631 is prepended automatically to any 7-digit numbers dialed. All instances of this must be changed to your preferred
area code. The other two modes are "NP" and "Alt". NP stands for "no prepend", to enter full 10-digit numbers, and "Alt" is the alternate mode. To minimize the total 
number of switches, the function of the bottom four buttons on the phone changes depending on the position of the mode switch.

Arduino IDE needs board support for 2560V. For this, add "MegaCore" board configuration from here: https://github.com/MCUdude/MegaCore
Also add the Adafruit EOD and the GxEPD2 libraries, which can be added from within the Arduino IDE by going to tools > Manage Libraries...

Use an AVR-ISP-MK2 to flash firmware. This is much easier than dealing with the USB port for programming. Just plug the programmer into the ICSP
header and use "Ctrl+Shift+U" within the Arduino IDE to compile and upload. No need to pick a dev assignment for the USB port.

The board settings under Tools in the Arduino IDE should be as follows (after installing the above MegaCore thingy):
Board: ATmega2560
Clock: External 8MHz
BOD: BOD 2.7v
Compiler LTO: LTO enabled
Pinout: Arduino MEGA pinout
Bootloader: Yes (UART0)

Requires hardware mod to FONA Tx connection, see forum "Firmware stuff" thread for details.
****************************************************/

#include <SoftwareSerial.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>

//For Adafruit e-ink display:
#define ENABLE_GxEPD2_GFX 0 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= 800 / (EPD::WIDTH / 8) ? EPD::HEIGHT : 800 / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_flex, MAX_HEIGHT(GxEPD2_213_flex)> display(GxEPD2_213_flex(25, 26, 28, 29)); // Pin order is ECS, D/C, RESET, BUSY. For Adafruit 2.13"
                                                                                                  // Flexible Monochrome EPD (AKA GDEW0213I5F)

//Define variables:
const byte nChars = 80;  // max number of character to buffer (may need more for texts). Note SoftwareSerial buffer limit of 63 chars
byte n = 1;   //For counting up numbers from the rotary dial for entering a digit in the phone number
byte k = 0;   //For specifying the digit in a phone number
unsigned long TimeSinceLastPulse = 0;   //used to see if enough of a delay has happened since the last pulse from the rotary dial to consider the sequence complete.
String buffer;          // String object buffer for incoming messages from FONA
byte PNumber[30]; // an array to store phone numbers as they're dialed with the rotary dial
bool StillOn = false;   // a flag to indicate that th
bool StartTimeSinceLastPulse = false;  //This gets sets to "true" the first time the rotary dial is used.
bool CallOn = false;    //Set to "true" when a call is in progress, to determine the function of the "call_startedn" pin.
bool newrotaryinput = false;
float fholder1;
int iholder;
int clvl;	  // call level storage integer
int rlvl; 	// ring level storage integer
float BattLevel;
float SigLevel;
int pagenum;		//holder for page number
int mode;	//1 = 631, 2 = NP, 3 = Alt. Marks the mode the phone's currently in. Needed for certain things.
unsigned long longTimer = 0;     // loop counter to do something periodically with a long interval.

byte rtcYear;
byte rtcMonth;
byte rtcDay;
byte rtcHour;
byte rtcMin;
byte dow;                     // Day of the week; 0 = Sunday, 6 = Saturday
String dowStr = "Invalid";
String monthStr = "time";
const int leadingDay[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};  // Array used for day of the week algorithm

//Define general output pins
const byte StatusLED = 13;
const byte HookLED = 49;
const byte BGLED1 = 7;    //bargraph LED 0 (D15)
const byte BGLED2 = 6;    //bargraph LED 1 (D14)
const byte BGLED3 = 16;   //bargraph LED 2 (D13)
const byte BGLED4 = 17;   //bargraph LED 3 (D12)
const byte BGLED5 = 14;   //bargraph LED 4 (D11)
const byte BGLED6 = 15;   //bargraph LED 5 (D10)
const byte BGLED7 = 42;   //bargraph LED 6 (D9)
const byte BGLED8 = 43;   //bargraph LED 7 (D8)
const byte BGLED9 = 47;   //bargraph LED 8 (D7)
const byte BGLED10 = 48;  //bargraph LED 9 (D6)
const byte FONAWake = A0;
const byte eink_ENA = 31;

//Define input pin
const byte HookButton = 33;       //Make call, hangup call, or answer incoming call (momentary switch shorts to gnd)
const byte ClearButton = 36;      //Clear number currently in dialout buffer from rotary dial (momentary switch gnd)
const byte SignalButton = 34;     //Hold to check signal strengh
const byte BatteryButton = 37;    //Hold to check battery level
const byte SaButton = 35;         //Hold to check battery level
const byte FnButton = 32;         //Hold to check battery level
const byte ModeSwitch_631 = 20;   //SP3T switch positioned to append a certain area code to all calls. NOTE: swap pin with ModeSwitch_alt for 1st rev boards
const byte ModeSwitch_NP = 19;	  //SP3T switch posiioned to "No Prepend" mode, in which a full 10-digit phone number is needed.
const byte ModeSwitch_alt = 18;   //SP3T switch positioned to switch to taking the alternate function of each button.
const byte RotaryPulseIn = 39;    //The pin that reads the state of the rotary dial.
const byte PowerState = A2;       //Reads the power state of the FONA module, 1=ON

SoftwareSerial FONAserial(12, 9); //Rx, Tx. Requires hardware mod, see forum "Firmware stuff" thread.

void setup(){
// Set output pin functions
	pinMode(StatusLED, OUTPUT);
	pinMode(HookLED, OUTPUT);
	pinMode(BGLED1, OUTPUT);
	pinMode(BGLED2, OUTPUT);
	pinMode(BGLED3, OUTPUT);
	pinMode(BGLED4, OUTPUT);
	pinMode(BGLED5, OUTPUT);
	pinMode(BGLED6, OUTPUT);
	pinMode(BGLED7, OUTPUT);
	pinMode(BGLED8, OUTPUT);
	pinMode(BGLED9, OUTPUT);
	pinMode(BGLED10, OUTPUT);
	pinMode(FONAWake, OUTPUT);
	pinMode(eink_ENA, OUTPUT);

	// Define input pin functions
	pinMode(RotaryPulseIn, INPUT_PULLUP);
	pinMode(HookButton, INPUT_PULLUP);
	pinMode(ClearButton, INPUT_PULLUP);
 	pinMode(SignalButton, INPUT_PULLUP);
	pinMode(BatteryButton, INPUT_PULLUP);
	pinMode(SaButton, INPUT_PULLUP);
	pinMode(FnButton, INPUT_PULLUP);
	pinMode(ModeSwitch_631, INPUT_PULLUP);
	pinMode(ModeSwitch_NP, INPUT_PULLUP);
	pinMode(ModeSwitch_alt, INPUT_PULLUP);

	digitalWrite(eink_ENA, HIGH);		//Pull the enable pin up on the e-ink display
  digitalWrite(FONAWake, HIGH);   //Default state for the FONA Power Key input

  buffer.reserve(nChars+1);       //Reserve space for FONA messages, may need increasing for texts but caution with SoftwareSerial buffer limit
	Serial.begin(115200);           //Hardware UART + FTDI easily handles this (despite 3.5% timing error with 8MHz clock)
	FONAserial.begin(9600);         //this can be increased this too, max tbc with logic analyser...

	// Check if FONA is ON, turn on if necessary.
  // The FONAwake pin toggles power so let's check if it's already on.
  if (digitalRead(PowerState) == LOW) {
    Serial.println("FONA is OFF");
    digitalWrite(FONAWake, LOW);    //Holding LOW for MIN 64ms wakes FONA  
    delay(128);
    digitalWrite(FONAWake, HIGH);
    while (PowerState == LOW) {}    //wait for FONA to power up, approx 5s
    Serial.println("FONA is ON");
  } else {
    Serial.println("FONA already ON");
  }	
	delay(6000);    // Can take up to 5-6s to start, reduce this if we do some other delaying stuff here, like a welcome display.

  // Initialise the FONA
  FONAserial.println("AT");           // Helps baud rate auto selection: https://en.wikipedia.org/wiki/Hayes_command_set#Autobaud
  Serial.println(FONAread(50));       // wait up to 50ms for start of reply then send reply over USB serial
  delay(13);
	FONAserial.println("AT+IPR=9600");	// Set baud rate on phone
	Serial.println(FONAread(50));
  delay(13);
	FONAserial.println("AT+CVHU=0");	  // Sets voice "hang up" control so that "ATH" disconnects voice calls.
	Serial.println(FONAread(50));
  delay(13);
	FONAserial.println("AT+CSDVC=3");  	// Set audio output channel. 3 is the speaker.
	Serial.println(FONAread(50));
  delay(13);
	FONAserial.println("AT+CRXGAIN=10000");	// Set Rx Gain, which affects the speaker volume during calls. This is a good value for use of the speaker as a handset.
	Serial.println(FONAread(50));
  delay(13);
	FONAserial.println("AT+CLVL=3");  	// Set volume (0-8)
	Serial.println(FONAread(50));
  delay(13);
	FONAserial.println("AT+CRSL=8");   	// Ringer volume (0-8)
  Serial.println(FONAread(50));
  delay(13);
  FONAserial.println("AT+CTZU=1");    // Enable automatic time & time zone update from cell network. This is stored in non-volatile...
  Serial.println(FONAread(50));       // memory, so after it's sent to the FONA once, these 2 lines can be commented out.

	n = 0;    //Starting phone number digit value is 0
	k = 0;    //Starting phone number digit position is 1

	display.init(115200);
	display.setRotation(0);
	display.setTextColor(GxEPD_BLACK);

	digitalWrite(StatusLED, HIGH);
	delay(500);
	digitalWrite(StatusLED, LOW);
	ClearBuffer();
  Serial.println(F("SETUP COMPLETE"));
}


void loop() {
  if (longTimer >= 100000) {               // Do something here periodically (max 4,294,967,295),
    longTimer = 0;                         // approx 100us per count (with no switch press delays)
    Serial.println(F("Long timer tick"));  // Here we update display with cell network time every 10s
    if (!(StartTimeSinceLastPulse == true || StillOn == true)) {  // Only update if dial not in use.
      displayTime();
    }
    //Serial.print("FONA: ");
    //Serial.println(FONAread(0));
  }
  longTimer++;
	
	if (CallOn == true){
		digitalWrite(HookLED, HIGH);
	}
	else if (CallOn == false){
		digitalWrite(HookLED, LOW);
	}

  if (TimeSinceLastPulse > 30000) {   // Interdigit pause? Approx 3s since last dialled number.
    StartTimeSinceLastPulse = false;  // reset here otherwise StartTimeSinceLastPulse never goes high after first...
    TimeSinceLastPulse = 0;           // dial and TimeSinceLastPulse counts forever - so reset here.
  }

	if (digitalRead(ModeSwitch_631) == LOW){
		mode = 1;
	}
 	else if (digitalRead(ModeSwitch_NP) == LOW){
		mode = 2;
	}
	else if (digitalRead(ModeSwitch_alt) == LOW){
		mode = 3;
	}

	//*********************ROTARY DIAL INPUT & FUNCTION BUTTON*******************************************************************************
	if (digitalRead(ModeSwitch_631) == LOW || digitalRead(ModeSwitch_NP) == LOW){
		if (digitalRead(FnButton) == LOW){
			StarPoundRotaryIn();	//This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
		}
		else{
			RotaryIn();   //This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
		}
	}
	else{
		RotaryIn();   //This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
	}

	//*********************CLEAR (C) BUTTON**************************************************************************
	if (digitalRead(ClearButton) == LOW){
		delay(200);
		//IF THE CLEAR BUTTON IS STILL DEPRESSED, CLEAR THE BUFFER
		if (digitalRead(ClearButton) == LOW){ 
			ClearBuffer();   //Clear whatever number has been entered so far
			digitalWrite(StatusLED, HIGH);
			delay(100);
			digitalWrite(StatusLED, LOW);
			delay(500);
		}
	}

	//*********************HOOK (H) BUTTON***************************************************************************
	if (digitalRead(HookButton) == LOW){   //Check CALL button
		//PLACE A CALL IF THE ROTARY HAS BEEN USED & AT LEAST 7 DIGITS WERE DIALED
		if (newrotaryinput = true && PNumber[6] != 99){
			if (digitalRead(ModeSwitch_631) == LOW){	//Check mode switch
				MakeCall631();   //Make a call using the number stored in the buffer as input from the rotary dial, prepended with 631
			}
			else {	//If in either No-Prepend or Alt mode
				MakeCall();   //Make a call using the number stored in the buffer as input from the rotary dial
			}
		}
		//ANSWER INCOMING CALL IF CALLON = FALSE
		else if (CallOn == false){
			FONAserial.println("ATA");
			//CallOn = true;	//!!!FIX!!!. The problem with turning the CallOn flag ON is that there's no check to see if a call was actually picked up.
		}
		delay(400);		
		//IF STILL HOLDING THE HOOK BUTTON BY ITSELF, HANGUP CALL REGARDLESS OF CALLON STATE
		if (digitalRead(HookButton) == LOW){ 
			FONAserial.println("ATH");
			delay(100);
			FONAserial.println("AT+CHUP");
			if (CallOn == true){		//If CallOn = true, make it false. This is just for the hook LED.
				CallOn = false;
			}
		}
	}

	//*********************S BUTTON**********************************************************************************
	if (digitalRead(SignalButton) == LOW){   //If the signal strength button is being depressed, go to the function that displays the signal strength with the LEDs
		//IF IN 631 MODE CHECK SIGNAL STRENGTH
		if (digitalRead(ModeSwitch_631) == LOW){
			mode = 1;
			SignalStrength();
		}
		//IF IN NP MODE CALL SPEED DIAL 1
		else if (digitalRead(ModeSwitch_NP) == LOW){
			mode = 2;
			ClearBufferSilent();
			pagenum = 5;
			PNumber[0] = 4;
			RetrieveContact();
			MakeCall();
		}
		//IF IN ALT MODE, SET CALL VOLUME
		else if (digitalRead(ModeSwitch_alt) == LOW){
			mode = 3;
			ClearBufferSilent();
			while (digitalRead(SignalButton) == LOW){
				RotaryIn();
			}
			if (PNumber[0] != 99){
				clvl=PNumber[0];
				FONAserial.print("AT+CLVL=");
				FONAserial.println(clvl);
				BarGraphSlow(PNumber[0]);
				delay(100);
				BarGraphSlow(PNumber[0]);
				delay(100);
				BarGraphSlow(PNumber[0]);
				ClearBufferSilent();
			}
		}
	}
	//*********************B BUTTON**********************************************************************************
	if (digitalRead(BatteryButton) == LOW){   
		//IF IN 631 MODE, CHECK BATTERY LEVEL
		if (digitalRead(ModeSwitch_631) == LOW){
			mode = 1;
			BatteryLevel();
		}
		//IF IN NP MODE, CALL SPEED DIAL 2
		else if (digitalRead(ModeSwitch_NP) == LOW){
			mode = 2;
			ClearBufferSilent();
			pagenum = 3;
			PNumber[0] = 4;
			RetrieveContact();
			MakeCall();
		}
		//IF IN ALT MODE, SET RING VOLUME
		else if (digitalRead(ModeSwitch_alt) == LOW){
			mode = 3;
			ClearBufferSilent();
			while (digitalRead(BatteryButton) == LOW){
				RotaryIn();
			}
			if (PNumber[0] != 99){
				rlvl = PNumber[0];
				FONAserial.print("AT+CRSL=");
				FONAserial.println(rlvl);
				BarGraphSlow(PNumber[0]);
				delay(100);
				BarGraphSlow(PNumber[0]);
				delay(100);
				BarGraphSlow(PNumber[0]);
				ClearBufferSilent();
			}
		}
	}

	//*********************Sa BUTTON**********************************************************************************
	if (digitalRead(SaButton) == LOW){   //I couldn't think of a name for this button. Sa doesn't have particular meaning.
		//IF IN 631 MODE, DO SOMETHING TBD
		if (digitalRead(ModeSwitch_631) == LOW){
			mode = 1;
			//placeholder
		}
		//IF IN NP MODE, CALL VOICEMAIL
		else if (digitalRead(ModeSwitch_NP) == LOW){
			mode = 2;
			ClearBufferSilent();
			pagenum = 8;
			PNumber[0] = 4;
			RetrieveContact();
			MakeCall();
		}
		//IF IN ALT MODE, CALL CONTACT FROM CURRENT PAGE
		else if (digitalRead(ModeSwitch_alt) == LOW){
			mode = 3;
			ClearBufferSilent();
			newrotaryinput = false;
			while (digitalRead(SaButton) == LOW){
				RotaryIn();
			}
			if (PNumber[0] != 99){
				RetrieveContact();
				MakeCall();			
				newrotaryinput = false;
				ClearBufferSilent();
			}
		}
	}

	//*********************ALT MODE (IN GENERAL)**********************************************************************************
	//In addition to the ALT MODE stuff specific to each button this section is needed for general stuff.
	//PREVENT CONTACTS PAGE FROM UPDATING IF THE ROTARY DIAL WAS USED BEFORE SWITCHING TO ALT MODE
	if (digitalRead(ModeSwitch_alt) == LOW){
		if ((mode == 1) || (mode == 2)){
			ClearBufferSilent();			
			newrotaryinput = false;	//Prevents display from updating upon entering ALT mode if a number was dialed previously but not used for another function.
		}
		mode = 3;
		//DISPLAY CONTACTS PAGES
		if (newrotaryinput == true){
			DisplayContacts();
			newrotaryinput = false;
			ClearBufferSilent();			
		}
	}
}
