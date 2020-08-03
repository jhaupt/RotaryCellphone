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
#include <Adafruit_SleepyDog.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include "bitmaps.h"

// ****** IMPORTANT: comment out next line if board does not have space for a vibration motor.
// Leave defined if the board has space but the motor is not installed, since there are mode switch wire changes.
#define HAS_VIBRATE

//For Adafruit e-ink display:
#define ENABLE_GxEPD2_GFX 0 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= 800 / (EPD::WIDTH / 8) ? EPD::HEIGHT : 800 / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_213_flex, MAX_HEIGHT(GxEPD2_213_flex)> display(GxEPD2_213_flex(25, 26, 28, 29)); // Pin order is ECS, D/C, RESET, BUSY. For Adafruit 2.13"
                                                                                                  // Flexible Monochrome EPD (AKA GDEW0213I5F)
																								  
//Define variables:
const byte nChars = 80; // Max number of character to buffer (may need more for texts). Note SoftwareSerial buffer limit of 63 chars
byte n = 0;             // For counting up numbers from the rotary dial for entering a digit in the phone number
byte k = 0;             // For specifying the digit in a phone number
unsigned long TimeSinceLastPulse = 0;   //used to see if enough of a delay has happened since the last pulse from the rotary dial to consider the sequence complete.
String buffer;          // String object buffer for incoming messages from FONA
String callerID;        // String object to store incoming caller ID
String prevCallerID;
byte PNumber[30];       // an array to store phone numbers as they're dialed with the rotary dial
bool StillOn = false;   // a flag to indicate that the dial pulse is continuing
bool StartTimeSinceLastPulse = false;  //This gets sets to "true" the first time the rotary dial is used.
bool CallOn = false;    //Set to "true" when a call is in progress, to determine the function of the "call_startedn" pin.
bool newrotaryinput = false;
float fholder1;
int iholder;
int clvl = 2;           // call level storage integer, 2 is the factory default, valid levels are 0-8
int rlvl = 4;           // ring level storage integer, 2 is the factory default, valid levels are 0-8
float BattLevel;
byte lowVccCount = 0;   // Count how many consective times a near exhausted battery condition has been read
float SigLevel;
int pagenum;            // holder for page number
int mode;               // 1 = 631, 2 = NP, 3 = Alt. Marks the mode the phone's currently in. Needed for certain things.
unsigned int shortTimer = 0;    // loop counter to do something approx once a second (check buffer for messages).
unsigned int longTimer = 0;     // loop counter to do something periodically, long interval, approx every 10s.
unsigned int veryLongTimer = 0; // loop counter to send FONA to sleep after a few mins of inactivity.
byte ringTimer = 0;             // Timer (seconds) for ringing alert, e.g. LED, vibration.
bool FONAsleepState = false;    // Flag to check for sleep mode for FONA (can still receive calls during sleep)

byte rtcYear = 0;
byte rtcMonth;
byte rtcDay;
byte rtcHour;
byte rtcMin;
byte prevRtcMin;                      // Used to test for next RTC minute
byte dow;                             // Day of the week; 0 = Sunday, 6 = Saturday
String dowStr = "----";               // Max 4 characters
String monthStr = "---------";        // Max 9 characters
String dateStr = "-----------------"; // Max 17 characters. Must initialise string otherwise the string addition operator might not run as expected
byte callHour = 0;                    // Time of last incoming call, hours
byte callMin = 0;                     // Time of last incoming call, minutes
const int leadingDay[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};  // Array used for day of the week algorithm

//Define general output pins
const byte StatusLED = 13;
const byte HookLED = 49;
const byte BGLED1 = 7;        // bargraph LED 0 (D15)
const byte BGLED2 = 6;        // bargraph LED 1 (D14)
const byte BGLED3 = 16;       // bargraph LED 2 (D13)
const byte BGLED4 = 17;       // bargraph LED 3 (D12)
const byte BGLED5 = 14;       // bargraph LED 4 (D11)
const byte BGLED6 = 15;       // bargraph LED 5 (D10)
const byte BGLED7 = 42;       // bargraph LED 6 (D9)
const byte BGLED8 = 43;       // bargraph LED 7 (D8)
const byte BGLED9 = 47;       // bargraph LED 8 (D7)
const byte BGLED10 = 48;      // bargraph LED 9 (D6)
const byte FONAWake = A0;     // FONA 'Key' pin, toggles power. NB Adafruit datasheet error; hold low for 64ms min to toggle power, not 5s.
const byte FONADTR = A1;      // FONA DTR pin, can be use to wake FONA from sleep.
const byte eink_ENA = 31;     // Leave enabled, power control is handled by the GxEPD2 library, disabling requires re-init

//Define input pin
const byte HookButton = 33;       // Make call, hangup call, or answer incoming call (momentary switch shorts to gnd)
const byte ClearButton = 36;      // Clear number currently in dialout buffer from rotary dial (momentary switch gnd)
const byte SignalButton = 34;     // Hold to check signal strengh
const byte BatteryButton = 37;    // Hold to check battery level
const byte SaButton = 35;         // Hold to check battery level
const byte FnButton = 32;         // Hold to check battery level
const byte RotaryPulseIn = 39;    // The pin that reads the state of the rotary dial.
const byte PowerState = A2;       // Reads the power state of the FONA module, 1=ON
const byte FONA_RI = 24;          // FONA Ring Indicator output, this stays high after receiving a call or text & must be reset with an AT command.
const byte ModeSwitch_NP = 19;    // SP3T switch posiioned to "No Prepend" mode, in which a full 10-digit phone number is needed.
#ifdef HAS_VIBRATE
const byte VibMotor = 5;          // Vibration motor pin, only available on early revision 3G boards, active low
const byte ModeSwitch_631 = 18;   // SP3T switch positioned to append a certain area code to all calls. NOTE: swap pin with ModeSwitch_alt for 1st rev boards
const byte ModeSwitch_alt = 20;   // SP3T switch positioned to switch to taking the alternate function of each button.
#else
const byte ModeSwitch_631 = 20;   // The '631' and Alt mode connections were reversed on the later board revision without the vibration motor
const byte ModeSwitch_alt = 18;
#endif

SoftwareSerial FONAserial(12, 9); // Rx, Tx. Requires hardware mod, see forum "Firmware stuff" thread.

void setup(){
#ifdef HAS_VIBRATE
pinMode(VibMotor, OUTPUT);
digitalWrite(VibMotor, HIGH);
#endif
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
  pinMode(FONADTR, OUTPUT);
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
  pinMode(FONA_RI, INPUT_PULLUP);

  digitalWrite(eink_ENA, HIGH);   // Pull the enable pin up on the e-ink display
  digitalWrite(FONAWake, HIGH);   // Default state for the FONA Power pin input
  digitalWrite(FONADTR, HIGH);    // Default state for the FONA DTR pin input

  buffer.reserve(nChars+1);       // Reserve space for FONA messages, may need increasing for texts but caution with SoftwareSerial buffer limit
  callerID.reserve(16);           // Reserve space for caller ID string (ITU 15 digits max + null), or 'Withheld' etc
  prevCallerID.reserve(16);       // Reserve space for to check whether Caller ID has been updated
  callerID = "none";              // Justine's original caller ID place holder, originally "???-????"
  Serial.begin(115200);           // Hardware UART + FTDI easily handles this (despite 3.5% timing error with 8MHz clock)
  FONAserial.begin(9600);         // this can be increased this too, max tbc with logic analyser...

  // Check if FONA is ON, turn on if necessary.
  // The FONA 'Key' pin toggles power so let's check if it's already on, this can save unnecessary delay.
  if (digitalRead(PowerState) == LOW) {
    Serial.printf(F("\nFONA is OFF\n"));
    digitalWrite(FONAWake, LOW);           // Holding LOW for MIN 64ms wakes FONA  
    delay(128);
    digitalWrite(FONAWake, HIGH);
    while (PowerState == LOW) {}           // wait for FONA to power on
    Serial.printf(F("\nFONA STARTING\n\n"));
    delay(6000);                           // Wait for FONA to initialise, up to 6s.
  } else {
    Serial.printf(F("\nFONA is already ON\n\n"));
    delay(500);                            // FONA already on & assumed initialised
  }

  display.init(115200);                    // Initialise and display a welcome screen
  welcomeDisplay();
  
  // Initialise the FONA
  FONAserial.println(F("AT"));             // Helps baud rate auto selection: https://en.wikipedia.org/wiki/Hayes_command_set#Autobaud
  Serial.println(FONAread(50));            // wait up to 50ms for start of reply then send reply over USB serial
  delay(50);
  FONAserial.println(F("AT+IPREX=9600"));  // Set baud rate on phone, takes a while this command.
  delay(200);                              // Not interested in the response, just clear the buffer.
  FONAread(0);
  FONAserial.println(F("ATI"));            // Get FONA identification information, including IMEI and
  buffer = FONAread(50);                   // store in buffer, it might be useful.
  Serial.println(buffer);
  delay(50);
  FONAserial.println(F("AT+CVHU=0"));      // Sets voice "hang up" control so that "ATH" disconnects voice calls.
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.println(F("AT+CSDVC=3"));     // Set audio output channel. 3 is the speaker.
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.println(F("AT+CRXGAIN=10000"));  // Set Rx Gain, (max ‭value 65,536‬) which affects the speaker volume during calls...
  Serial.println(FONAread(50));            // 10000 is a good value for use of the speaker as a handset (if aged under 50)!
  delay(50);
  FONAserial.print(F("AT+CLVL="));         // Set volume (0-8)
  FONAserial.println(clvl);
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.print(F("AT+CRSL="));         // Ringer volume (0-8)
  FONAserial.println(rlvl);
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.println(F("AT+CLIP=1"));      // Enable 'Calling line identification presentation' (caller ID messages)
  Serial.println(FONAread(50));
  delay(50);
  // ******  The following AT commands are stored in non-volatile memory:
  FONAserial.println(F("AT+CTZU=1"));      // Enable automatic time & time zone update from cell network.
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.print(F("AT+CRSL="));         // Reset ring level else we might end up with muted ringer and no vibration.
  FONAserial.println(rlvl);                // Default ring level.
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.print(F("AT+CLVL="));         // Reset ring level else we might end up with muted ringer and no vibration.
  FONAserial.println(clvl);                // Default ring level.
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.println(F("AT+CFGRI=1,1"));   // Enable pulldown of RI pin on URC (Unsolicited Result Code)
  Serial.println(FONAread(50));
  delay(50);
  FONAserial.println(F("AT+CRIRS"));       // Clear FONA RI pin (active low)
  Serial.println(FONAread(50));

  digitalWrite(StatusLED, HIGH);           // FONA setup done, blink status LED.
  delay(500);
  digitalWrite(StatusLED, LOW);
  ClearBuffer();
  Serial.printf(F("SETUP COMPLETE\n\n"));

  // **** If C button held, go to UART passthrough mode for testing ****
  // **** Will display FONA info and IMEI on ePaper display.        ****
  // **** Use PuTTY at 115200 for communication (or similar),       ****
  // **** anything type on the serial terminal will go to the FONA  ****
  // **** and vice versa until the power is cycled.                 ****
  if (digitalRead(ClearButton) == LOW) {
    display.setRotation(1);                 // Display passthrough mode confirmation and FONA info, IMEI etc
    display.setTextColor(GxEPD_BLACK);
    display.firstPage();
    do {
      display.setFont();                    // Standard (tiny) font
      display.setCursor(0, 0);
      display.println(F("PASSTHROUGH MODE:"));
      display.println(buffer);              // Print FONA info to the display, should be in buffer.
      display.println(F("Cycle power to quit"));
    } while (display.nextPage());
    Serial.println(F("PASSTHROUGH MODE"));
    while (true) {                          // Stay here forever (until power cycle)
      if (Serial.available())               // If anything comes in Serial (USB),
        FONAserial.write(Serial.read());    // read it and send it to the FONA

      if (FONAserial.available())           // If anything comes in from the FONA
        Serial.write(FONAserial.read());    // read it and send it out Serial (USB)
    }
  }
}


void loop() {

  if (CallOn == true){
    digitalWrite(HookLED, HIGH);
  }
  else if (CallOn == false){
    digitalWrite(HookLED, LOW);
  }

  if (digitalRead(ModeSwitch_631) == LOW)
    mode = 1;
  else if (digitalRead(ModeSwitch_NP) == LOW)
    mode = 2;
  else if (digitalRead(ModeSwitch_alt) == LOW)
    mode = 3;

  //if (digitalRead(FONA_RI) == LOW)      // Display state of FONA RI for debugging
  //  digitalWrite(BGLED10, HIGH);
  //else
  //  digitalWrite(BGLED10, LOW);


  // ***************************** Periodic stuff *****************************
  // **** short interval, approx every second  ****
  // **** check FONA for URC (eg ring message) ****
  if (shortTimer > 9430) {                               // Do something quick every 1s (<5ms so as to not affect dial timing)
    shortTimer = 0;                                      // Hint: this would also be a good place to insert call state parsing
    //Serial.println(F("Short timer tick"));             // Optional for debugging
    checkAlerts();                                       // Check for an alert from the FONA, e.g. incoming call? Caller ID?
    if (ringTimer > 0) {                                 // Turn on vibration if 'RING' received from FONA & ring muted.
      digitalWrite(StatusLED, HIGH);                     // Flask LED for incoming call.
      #ifdef HAS_VIBRATE
      if (rlvl == 0)                                     // Vibrate if ringer at level 0 (off), no problem if motor not installed.
        digitalWrite(VibMotor, LOW);                     // Vibration on when pin is low, cadence mimics the ringing sound:
      #endif
      ringTimer --;                                      // On 2s, off 4s.
    }
    else {
      ringTimer = 0;
      digitalWrite(StatusLED, LOW);                      // Turn of LED.
      #ifdef HAS_VIBRATE
      digitalWrite(VibMotor, HIGH);                      // Turn off vibration motor (negative logic).
      #endif
    }
    if (FONAsleepState == true)                          // If FONA should be sleeping, force its Rx pin low to enable auto sleep (normal RS232 idle state is high)
      PORTH = PORTH & 0b10111111;                        // sorry but direct port manipulation is 100x faster than: digitalWrite(9, LOW)
    longTimer++;
    veryLongTimer++;
  }
  shortTimer++;

  // **** long interval, approx every 10 seconds ****
  // **** update time and CID if required        ****
  // **** fast check battery is OK or shutdown   ****
  if (longTimer > 10) {                                           // Do something here periodically approx ever 10s...
    longTimer = 0;                                                // We can do something that takes more than 5ms only if the dial not in use.
    // Serial.println(F("Long timer tick"));                      // Here we update display with cell network time every 10s.
    // if (!(StartTimeSinceLastPulse == true || StillOn == true) && FONAsleepState == false) { 
    if (StartTimeSinceLastPulse == false && StillOn == false && FONAsleepState == false) { // do periodic stuff if the dial is not in use and the FONA not sleeping:
      displayCID();                                               // Display last caller ID, or 'none', or 'witheld. Tiny font to allow 16 digits.
      displayTime();                                              // Display date & time if the minute has changed.
      if (readVcc() > 341) {                                      // *** GRACEFUL POWERDOWN when battery is almost exhausted ***
        lowVccCount++;                                            // Check whether battery voltage has consistantly been very low (< 3.3V). 
        Serial.printf("\nLOW BATTERY WARNING %d\n", lowVccCount); // If so; powerdown FONA, display powerdown time, blink hook LED every 4s.
        if (lowVccCount > 5) {                                    // Note: 3.3V is the min supply voltage for the FONA,
          Serial.printf(F("\nLOW BATTERY POWERDOWN\n"));          // readVcc() is much faster (2.1ms) than using the FONA battery status check.
          shutdownPhone();                                        // This fast read is very unlikely to affect loop timing of the rotary dial.
        }                                                         // Note: readVcc() returns a value inversely proportional to Vcc (the '>' is correct)
      }                                                           // Vcc (mV) = 1125300 / readVcc()
      else
        lowVccCount = 0;
    }
  }

  // **** very long interval, approx every 5 minutes ****
  if (veryLongTimer > 300) {           // count in approx 1 second steps
    veryLongTimer = 0;
    FONAsleep();   
  }
  
  // ***************************** End periodic stuff *****************************


  if (TimeSinceLastPulse > 20000) {   // Inter number pause? Approx 2s since last dialled number?
    StartTimeSinceLastPulse = false;  // reset here otherwise StartTimeSinceLastPulse never goes high after first...
    TimeSinceLastPulse = 0;           // dial and TimeSinceLastPulse counts forever - so reset here.
  }


  //*********************ROTARY DIAL INPUT & FUNCTION BUTTON*******************************************************************************
  if (digitalRead(ModeSwitch_631) == LOW || digitalRead(ModeSwitch_NP) == LOW){
    if (digitalRead(FnButton) == LOW) {
      StarPoundRotaryIn();  //This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
    }
    else {
      RotaryIn();   //This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
    }
  }
  else {
    RotaryIn();     //This function doesn't loop on it's own, but rather depends on the main "void loop" to do its work.
  }

  //*********************CLEAR (C) BUTTON**************************************************************************
  if (digitalRead(ClearButton) == LOW) {
    delay(200);
    //IF THE CLEAR BUTTON IS STILL DEPRESSED, CLEAR THE BUFFER
    if (digitalRead(ClearButton) == LOW) {
      ClearBuffer();                         //Clear whatever number has been entered so far
      digitalWrite(StatusLED, HIGH);
      delay(500);
      digitalWrite(StatusLED, LOW);
      delay(500);
      if (digitalRead(ClearButton) == LOW) { // If button still held, wake the FONA or reload the welcome display as appropriate
        if (FONAsleepState == true) {
          FONAwake();
          forceUpdate();                     // Forces time update and immediate refresh of time and CID display.
        }
        else
          welcomeDisplay();
      }
    }
  }

  //*********************HOOK (H) BUTTON***************************************************************************
  if (digitalRead(HookButton) == LOW){   //Check CALL button
    //PLACE A CALL IF THE ROTARY HAS BEEN USED & AT LEAST 3 DIGITS WERE DIALED
    if (newrotaryinput = true && PNumber[2] != 99){
      if (digitalRead(ModeSwitch_631) == LOW) {  //Check mode switch
        MakeCall631();      //Make a call using the number stored in the buffer as input from the rotary dial, prepended with 631
      }
      else {  //If in either No-Prepend or Alt mode
        MakeCall();   //Make a call using the number stored in the buffer as input from the rotary dial
      }
    }
    //ANSWER INCOMING CALL IF CALLON = FALSE
    else if (CallOn == false){
      FONAserial.println(F("ATA"));
      //CallOn = true;      // The problem with turning the CallOn flag ON is that there's no check to see if a call was actually picked up.
    }                       // FIXED! By testing for call begin and end messages every 1s with the queryAlert() function.
    delay(800);             // Don't make this delay too short or we risk answering a call & immediately disconnecting!
    //IF STILL HOLDING THE HOOK BUTTON BY ITSELF, HANGUP CALL REGARDLESS OF CALLON STATE
    if (digitalRead(HookButton) == LOW) { 
      FONAserial.println(F("ATH"));
      delay(100);
      FONAserial.println(F("AT+CHUP"));
      if (CallOn == true){  //If CallOn = true, make it false. This is just for the hook LED.
        CallOn = false;
      }
    }
  }

  //*********************S BUTTON**********************************************************************************
  if (digitalRead(SignalButton) == LOW){   //If the signal strength button is being depressed, go to the function that displays the signal strength with the LEDs
    //IF IN 631 MODE CHECK SIGNAL STRENGTH
    if (digitalRead(ModeSwitch_631) == LOW) {
      mode = 1;
      SignalStrength();
    }
    //IF IN NP MODE CALL SPEED DIAL 1
    else if (digitalRead(ModeSwitch_NP) == LOW) {
      mode = 2;
      ClearBufferSilent();
      pagenum = 5;
      PNumber[0] = 4;
      RetrieveContact();
      MakeCall();
    }
    //IF IN ALT MODE, SET CALL VOLUME
    else if (digitalRead(ModeSwitch_alt) == LOW) {
      mode = 3;
      ClearBufferSilent();
      while (digitalRead(SignalButton) == LOW){
        RotaryIn();
      }
      if (PNumber[0] != 99) {
        clvl=PNumber[0];
        if (clvl > 7)                    // valid ring level is 0-8
          clvl = 8;
        BarGraphSlow(PNumber[0]);
        delay(100);
        BarGraphSlow(PNumber[0]);
        delay(100);
        BarGraphSlow(PNumber[0]);
        ClearBufferSilent();
        Serial.print(F("Call volume: "));
        Serial.println(clvl);
      }
    }
  }
  //*********************B BUTTON**********************************************************************************
  if (digitalRead(BatteryButton) == LOW) {   
    //IF IN 631 MODE, CHECK BATTERY LEVEL
    if (digitalRead(ModeSwitch_631) == LOW) {
      mode = 1;
      BatteryLevel();
      delay(800);
    }
    //IF IN NP MODE, CALL SPEED DIAL 2
    else if (digitalRead(ModeSwitch_NP) == LOW) {
      mode = 2;
      ClearBufferSilent();
      pagenum = 3;
      PNumber[0] = 4;
      RetrieveContact();
      MakeCall();
    }
    //IF IN ALT MODE, SET RING VOLUME
    else if (digitalRead(ModeSwitch_alt) == LOW) {
      mode = 3;
      ClearBufferSilent();
      while (digitalRead(BatteryButton) == LOW){
        RotaryIn();
      }
      if (PNumber[0] != 99) {
        rlvl = PNumber[0];
        if (rlvl > 7)                    // valid ring level is 0-8
          rlvl = 8;
        BarGraphSlow(PNumber[0]);
        delay(100);
        BarGraphSlow(PNumber[0]);
        delay(100);
        BarGraphSlow(PNumber[0]);
        FONAserial.print(F("AT+CRSL="));
        FONAserial.println(rlvl);
        ClearBufferSilent();
        Serial.print(F("Ring level: "));
        Serial.println(rlvl);
      }
    }
  }

  //*********************Sa BUTTON**********************************************************************************
  if (digitalRead(SaButton) == LOW){   //I couldn't think of a name for this button. Sa doesn't have particular meaning.
    //IF IN 631 MODE, DO SOMETHING TBD
    if (digitalRead(ModeSwitch_631) == LOW) {
      mode = 1;
      //placeholder
    }
    //IF IN NP MODE, CALL VOICEMAIL
    else if (digitalRead(ModeSwitch_NP) == LOW) {
      mode = 2;
      ClearBufferSilent();
      pagenum = 8;
      PNumber[0] = 4;
      RetrieveContact();
      MakeCall();
    }
    //IF IN ALT MODE, CALL CONTACT FROM CURRENT PAGE
    else if (digitalRead(ModeSwitch_alt) == LOW) {
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
      newrotaryinput = false;  //Prevents display from updating upon entering ALT mode if a number was dialed previously but not used for another function.
    }
    mode = 3;
    //DISPLAY CONTACTS PAGES
    if (newrotaryinput == true) {
      DisplayContacts();
      newrotaryinput = false;
      ClearBufferSilent();      
    }
  }
}
