void RotaryIn() {  //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
  if (digitalRead(RotaryPulseIn) == HIGH) {   //If the pin connected to the rotary dial goes high...
    StartTimeSinceLastPulse = true;
    if (StillOn == false) {    //...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
      BarGraphFast(n + 1);
      n++;    //...Increment the counter...
      if (n == 10) {
        n = 0;
      }
      delay(10);
    }
    StillOn = true;   // ...and turn on the flag that says a pulse just happened.
  }

  if (StillOn == true) {    //If a pulse just happened...
    if ((digitalRead(RotaryPulseIn) == LOW)) {  //See if the pin connected to the rotary dial goes low...
      StillOn = false;    //...if it did, we turn off the "StillOn" flag.
      digitalWrite(StatusLED, HIGH);
      delay(20);
      digitalWrite(StatusLED, LOW);
      delay(40);
      TimeSinceLastPulse = 0;     //Reset the time since last pulse (not really time... rather a counter for "void loop").
    }
  }

  if (StartTimeSinceLastPulse == true) {
    TimeSinceLastPulse++;
  }

  if (TimeSinceLastPulse == 1000) {   //If a long enough time has lapsed since the last pulse from the rotary dial, assume the dial has stopped rotating...
    PNumber[k] = n;    //...and write the current value of n to the current position (k) in the phone number (PNumber)
    k++;      //increment to the next position of the phone number
    FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
    if (n != 0)
      FONAserial.println(n);
    else
      FONAserial.println(10);
    delay(20);
    FONAserial.print("AT+VTS=");	//Send DTMF tone over network (for menu entries, etc).
    FONAserial.println(n);
    newrotaryinput = true;
    n = 0;  //reset n
  }
}

void StarPoundRotaryIn() {  //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
  if (digitalRead(RotaryPulseIn) == HIGH) {   //If the pin connected to the rotary dial goes high...
    StartTimeSinceLastPulse = true;
    if (StillOn == false) {    //...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
      BarGraphFast(n + 1);
      n++;    //...Increment the counter...
      if (n == 10) {
        n = 0;
      }
      delay(10);
    }
    StillOn = true;   // ...and turn on the flag that says a pulse just happened.
  }

  if (StillOn == true) {    //If a pulse just happened...
    if ((digitalRead(RotaryPulseIn) == LOW)) {  //See if the pin connected to the rotary dial goes low...
      StillOn = false;    //...if it did, we turn off the "StillOn" flag.
      digitalWrite(StatusLED, HIGH);
      delay(20);
      digitalWrite(StatusLED, LOW);
      delay(40);
      TimeSinceLastPulse = 0;     //Reset the time since last pulse (not really time... rather a counter for "void loop").
    }
  }

  if (StartTimeSinceLastPulse == true) {
    TimeSinceLastPulse++;
  }

  if (TimeSinceLastPulse == 1000) {   //If a long enough time has lapsed since the last pulse from the rotary dial, assume the dial has stopped rotating...
    PNumber[k] = n;    //...and write the current value of n to the current position (k) in the phone number (PNumber)
    k++;      //increment to the next position of the phone number
    if (n == 2) {		//Consider this a "*"
      FONAserial.println("AT+CPTONE=*");	//Play DTMF tone over speaker
      delay(20);
      FONAserial.println("AT+VTS=*");	//Send DTMF tone over network (for menu entries, etc).
    }
    else if (n == 1) {	//Consider this a "#"
      FONAserial.println("AT+CPTONE=#");	//Play DTMF tone over speaker
      delay(20);
      FONAserial.println("AT+VTS=#");	//Send DTMF tone over network (for menu entries, etc).
    }
    n = 0;  //reset n
  }
}



void FONAserialReceive() {  //Listens over RS232, puts any string that's received into "ReceivedChars", and looks for some EndMarker character to change the NewData flag to True.
  static byte n = 0;
  char EndMarker = 'O';     //This is the "K" from "OK", which the SIM5230 always returns after accepting a command.
  char rc;
  int cnt;
  while (FONAserial.available() && NewData == false) {		//For some reason the conditional MUST be "FONAserial.available()" and not "FONAserial.available() > 0".
    rc = FONAserial.read();
    if (rc != EndMarker) {
      ReceivedChars[n] = rc;
      n++;
      if (n >= nChars) {
        n = nChars - 1;
      }
    }
    else {
      ReceivedChars[n] = '\0'; // terminate the string
      n = 0;
      NewData = true;
    }
  }
}

// READ FONA serial (Steve's method):
// Return all serial data available from the FONA as a String object. The argument is the time to wait (ms) for 
// the first character, can be zero or 13ms upwards (the battery would go flat before max delay). A small wait is 
// useful where there might be a delay in FONA response. Note the SoftwareSerial buffer length is 63 characters.
// Thanks to Cristian Steib for the SIM800L library and Vittorio Esposito for SIM800L(revised) from whom this
// method/function is derived: https://github.com/VittorioEsposito/Sim800L-Arduino-Library-revised
String FONAread(long int timeout) {
  long int timeOld = millis();
  while (!(FONAserial.available()) && !(millis() > timeOld + timeout)) {
    delay(13);
  }
  String str;
  while(FONAserial.available()) {
    if (FONAserial.available() > 0) {
      str += (char) FONAserial.read();
    }
  }
  return str;
}

// Set FONA baud rate, target 4800 baud
// Try common baud rates; 4800, 9600, ~115200
// Because if baud rate has been stored to non-volatile memory, auto baud detection will not work.
// Set the baud rate to non-volatile memory if necessary (AT+IPREX=0 for auto baud)
void setFONAbaud() {
  Serial.println(F("\nChecking FONA baud rate:"));
  FONAserial.begin(4800);                    // Try 4800 baud
  Serial.println(F("Trying 4800"));
  FONAread(0);                               // flush serial receive buffer
  FONAserial.println("\nAT");                // send AT to the FONA
  buffer = FONAread(80);                     // and look for an OK response
  Serial.println(buffer);
  if (buffer.indexOf("OK") > -1) {           // baud rate already set, exit function
    Serial.println(F("Baud rate already 4800"));
    return;
  }
  FONAserial.begin(9600);                    // or try 9600 baud
  Serial.println(F("Trying 9600"));
  FONAread(0);                               // flush serial receive buffer
  FONAserial.println("\nAT");                // send AT to the FONA
  buffer = FONAread(80);                     // and look for an OK respons
  Serial.println(buffer);
  if (buffer.indexOf("OK") > -1) {           // power-on baud rate was 9600
    FONAserial.println(F("AT+IPREX=4800"));  // set 4800 baud rate & exit function
    FONAserial.begin(4800);
    Serial.println(F("Baud rate set 4800"));
    delay(250);
    return;
  }
  FONAserial.begin(115200);                  // or try ~115200 baud, the ATMEGA struggles with this
  Serial.println(F("Trying 115200"));
  FONAread(0);                               // flush serial receive buffer
  FONAserial.println("\nAT");                // send AT to the FONA
  buffer = FONAread(80);                     // and look for an OK respons
  Serial.println(buffer);
  if (buffer.indexOf("OK") > -1) {           // power-on baud rate was 115200
    FONAserial.println(F("AT+IPREX=4800"));  // set 4800 baud rate & exit function
    FONAserial.begin(4800);
    Serial.println(F("Baud rate set 4800"));
    delay(250);
    return;
  }
  Serial.println(F("FONA baud rate checks failed"));
}

void ClearBuffer() { //Clear the currently entered phone number
  PNumber[0] = 99;  //Reset the phone number
  PNumber[1] = 99;
  PNumber[2] = 99;
  PNumber[3] = 99;
  PNumber[4] = 99;
  PNumber[5] = 99;
  PNumber[6] = 99;
  PNumber[7] = 99;
  PNumber[8] = 99;
  PNumber[9] = 99;
  PNumber[10] = 99;
  PNumber[11] = 99;
  PNumber[12] = 99;
  PNumber[13] = 99;
  PNumber[14] = 99;
  PNumber[15] = 99;
  k = 0;    //Reset the position of the phone number
  newrotaryinput = false;
  BarGraphWipeUp();
}

void ClearBufferSilent() { //Clear the currently entered phone number
  PNumber[0] = 99;  //Reset the phone number
  PNumber[1] = 99;
  PNumber[2] = 99;
  PNumber[3] = 99;
  PNumber[4] = 99;
  PNumber[5] = 99;
  PNumber[6] = 99;
  PNumber[7] = 99;
  PNumber[8] = 99;
  PNumber[9] = 99;
  PNumber[10] = 99;
  PNumber[11] = 99;
  PNumber[12] = 99;
  PNumber[13] = 99;
  PNumber[14] = 99;
  PNumber[15] = 99;
  newrotaryinput = false;
  k = 0;    //Reset the position of the phone number
}

void BarGraphSlow(int level) {
  Serial.println(level);
  if (level >= 1) {
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2) {
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3) {
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4) {
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5) {
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6) {
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7) {
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8) {
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9) {
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10) {
    digitalWrite(BGLED10, HIGH);
  }
  delay(200);
  digitalWrite(BGLED1, LOW);
  digitalWrite(BGLED2, LOW);
  digitalWrite(BGLED3, LOW);
  digitalWrite(BGLED4, LOW);
  digitalWrite(BGLED5, LOW);
  digitalWrite(BGLED6, LOW);
  digitalWrite(BGLED7, LOW);
  digitalWrite(BGLED8, LOW);
  digitalWrite(BGLED9, LOW);
  digitalWrite(BGLED10, LOW);
  delay(100);
}

void BarGraphFast(int level) {
  Serial.println(level);
  if (level >= 1) {
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2) {
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3) {
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4) {
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5) {
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6) {
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7) {
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8) {
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9) {
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10) {
    digitalWrite(BGLED10, HIGH);
  }
  delay(20);
  digitalWrite(BGLED1, LOW);
  digitalWrite(BGLED2, LOW);
  digitalWrite(BGLED3, LOW);
  digitalWrite(BGLED4, LOW);
  digitalWrite(BGLED5, LOW);
  digitalWrite(BGLED6, LOW);
  digitalWrite(BGLED7, LOW);
  digitalWrite(BGLED8, LOW);
  digitalWrite(BGLED9, LOW);
  digitalWrite(BGLED10, LOW);
}

void BarGraphMed(int level) {
  Serial.println(level);
  if (level >= 1) {
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2) {
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3) {
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4) {
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5) {
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6) {
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7) {
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8) {
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9) {
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10) {
    digitalWrite(BGLED10, HIGH);
  }
  delay(80);
  digitalWrite(BGLED1, LOW);
  digitalWrite(BGLED2, LOW);
  digitalWrite(BGLED3, LOW);
  digitalWrite(BGLED4, LOW);
  digitalWrite(BGLED5, LOW);
  digitalWrite(BGLED6, LOW);
  digitalWrite(BGLED7, LOW);
  digitalWrite(BGLED8, LOW);
  digitalWrite(BGLED9, LOW);
  digitalWrite(BGLED10, LOW);
  delay(50);
}

void BarGraphWipeUp() {
  int holdtime = 20;
  digitalWrite(StatusLED, HIGH);
  digitalWrite(BGLED10, HIGH);
  delay(holdtime);
  digitalWrite(BGLED10, LOW);
  digitalWrite(BGLED9, HIGH);
  delay(holdtime);
  digitalWrite(BGLED9, LOW);
  digitalWrite(BGLED8, HIGH);
  delay(holdtime);
  digitalWrite(BGLED8, LOW);
  digitalWrite(StatusLED, LOW);
  digitalWrite(BGLED7, HIGH);
  delay(holdtime);
  digitalWrite(BGLED7, LOW);
  digitalWrite(BGLED6, HIGH);
  delay(holdtime);
  digitalWrite(BGLED6, LOW);
  digitalWrite(BGLED5, HIGH);
  delay(holdtime);
  digitalWrite(BGLED5, LOW);
  digitalWrite(BGLED4, HIGH);
  delay(holdtime);
  digitalWrite(BGLED4, LOW);
  digitalWrite(BGLED3, HIGH);
  delay(holdtime);
  digitalWrite(BGLED3, LOW);
  digitalWrite(BGLED2, HIGH);
  delay(holdtime);
  digitalWrite(BGLED2, LOW);
  digitalWrite(BGLED1, HIGH);
  delay(holdtime);
  digitalWrite(BGLED1, LOW);
}

void BarGraphWipeDown() {
  int holdtime = 20;
  digitalWrite(BGLED1, HIGH);
  delay(holdtime);
  digitalWrite(BGLED1, LOW);
  digitalWrite(BGLED2, HIGH);
  delay(holdtime);
  digitalWrite(BGLED2, LOW);
  digitalWrite(BGLED3, HIGH);
  delay(holdtime);
  digitalWrite(BGLED3, LOW);
  digitalWrite(BGLED4, HIGH);
  delay(holdtime);
  digitalWrite(BGLED4, LOW);
  digitalWrite(BGLED5, HIGH);
  delay(holdtime);
  digitalWrite(BGLED5, LOW);
  digitalWrite(BGLED6, HIGH);
  delay(holdtime);
  digitalWrite(BGLED6, LOW);
  digitalWrite(BGLED7, HIGH);
  delay(holdtime);
  digitalWrite(BGLED7, LOW);
  digitalWrite(BGLED8, HIGH);
  delay(holdtime);
  digitalWrite(BGLED8, LOW);
  digitalWrite(BGLED9, HIGH);
  delay(holdtime);
  digitalWrite(BGLED9, LOW);
  digitalWrite(BGLED10, HIGH);
  delay(holdtime);
  digitalWrite(BGLED10, LOW);
}

void BatteryLevel() {
  while (digitalRead(BatteryButton) == LOW) {  //Loop for as long as the battery level button is depressed.
    FONAserial.println("AT+CBC");
    FONAserialReceive();
    delay(10);
    if (NewData == true) {
      digitalWrite(StatusLED, HIGH);
      lhlf = (int)ReceivedChars[20] - 48;
      BarGraphSlow(lhlf);
      Serial.print(ReceivedChars);
      Serial.print(ReceivedChars[20]);
      Serial.println(ReceivedChars[21]);
      NewData = false;
      digitalWrite(StatusLED, LOW);
    }
  }
}

void SignalStrength() {
  while (digitalRead(SignalButton) == LOW) {  //Loop for as long as the signal strength button is depressed.
    FONAserial.println("AT+CSQ");		//FONA returns signal strength from 0-31
    Serial.println("AT+CSQ");
    FONAserialReceive();
    delay(100);
    if (NewData == true) {
      digitalWrite(StatusLED, HIGH);
      lhlf = (int)ReceivedChars[18] - 48;
      //rhlf = (int)ReceivedChars[19] - 48;
      //fholder1 = (10*lhlf)+rhlf;
      SigLevel = lhlf;		//10*(fholder1/31);		//Signal strength is 0-31: 0 is < -113dBm and 31 is > -51dBm
      Serial.println(ReceivedChars);
      Serial.println(SigLevel);
      BarGraphSlow(SigLevel);
      //Serial.println(ReceivedChars[18]);
      //Serial.println(ReceivedChars[19]);
      NewData = false;
      digitalWrite(StatusLED, LOW);
    }
  }
}

void MakeCall631() {
  ToneReport();
  FONAserial.print("ATD631");
  Serial.print("ATD631");
  FONAserial.print(PNumber[0]);
  Serial.print(PNumber[0]);
  FONAserial.print(PNumber[1]);
  Serial.print(PNumber[1]);
  FONAserial.print(PNumber[2]);
  Serial.print(PNumber[2]);
  FONAserial.print(PNumber[3]);
  Serial.print(PNumber[3]);
  FONAserial.print(PNumber[4]);
  Serial.print(PNumber[4]);
  FONAserial.print(PNumber[5]);
  Serial.print(PNumber[5]);
  FONAserial.print(PNumber[6]);
  Serial.print(PNumber[6]);
  FONAserial.println(";");
  Serial.print(";");
  delay(10);
  FONAserialReceive();
  digitalWrite(StatusLED, HIGH);
  delay(500);
  digitalWrite(StatusLED, LOW);
  NewData = false;
  CallOn = true;
  ClearBufferSilent();
}

void MakeCall() {
  ToneReport();
  CallOn = true;
  FONAserial.print("ATD");
  Serial.print("ATD");
  FONAserial.print(PNumber[0]);
  Serial.print(PNumber[0]);
  FONAserial.print(PNumber[1]);
  Serial.print(PNumber[1]);
  FONAserial.print(PNumber[2]);
  Serial.print(PNumber[2]);
  FONAserial.print(PNumber[3]);
  Serial.print(PNumber[3]);
  FONAserial.print(PNumber[4]);
  Serial.print(PNumber[4]);
  FONAserial.print(PNumber[5]);
  Serial.print(PNumber[5]);
  FONAserial.print(PNumber[6]);
  Serial.print(PNumber[6]);
  FONAserial.print(PNumber[7]);
  Serial.print(PNumber[7]);
  FONAserial.print(PNumber[8]);
  Serial.print(PNumber[8]);
  FONAserial.print(PNumber[9]);
  Serial.print(PNumber[9]);
  FONAserial.println(";");
  Serial.println(";");
  delay(100);
  FONAserialReceive();
  digitalWrite(StatusLED, HIGH);
  delay(500);
  digitalWrite(StatusLED, LOW);
  NewData = false;
  CallOn = true;
  ClearBufferSilent();
}

void ToneReport() {
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[0]);
  BarGraphMed(PNumber[0]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[1]);
  BarGraphMed(PNumber[1]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[2]);
  BarGraphMed(PNumber[2]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[3]);
  BarGraphMed(PNumber[3]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[4]);
  BarGraphMed(PNumber[4]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[5]);
  BarGraphMed(PNumber[5]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[6]);
  BarGraphMed(PNumber[6]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[7]);
  BarGraphMed(PNumber[7]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[8]);
  BarGraphMed(PNumber[8]);
  FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
  FONAserial.println(PNumber[9]);
  BarGraphMed(PNumber[9]);
}
