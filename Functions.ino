void welcomeDisplay() {                                            // Put you startup/welcome/reset screen here
  display.setRotation(0);                                          // add a help msg or personal message
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();                                             // Display a welcome graphic
  do {
    display.drawBitmap(0, 30, sleepy_moon, 104, 76, GxEPD_BLACK);  // Set the array name to the preferred bitmap image
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(2, 122); 
    display.print(F("Rotary"));
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(3, 144); 
    display.print(F("CellPhone"));
    display.setFont();                                             // Back to standard (tiny) font
    display.setCursor(2, 155);
    display.print(F("made in Cambridge"));                         // Personalise here
  } while (display.nextPage());
  forceUpdate();                                                   // Update Time & Caller ID on next main loop pass
}

void forceUpdate() {                                               // Force time & caller ID update on next main loop pass
  longTimer = 99;
  prevRtcMin = 99;
  prevCallerID = "";                                               // Null string
}

void RotaryIn(){  //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
  if (digitalRead(RotaryPulseIn) == HIGH){  //If the pin connected to the rotary dial goes high...
    StartTimeSinceLastPulse = true;
    if (StillOn == false){                  // ...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
      BarGraphFast(n+1);
      n++;                                  //...Increment the counter...
      if (n == 10){
        n = 0;
      }
      delay(10);
    }
    StillOn = true;                         // ...and turn on the flag that says a pulse just happened.
  }

  if (StillOn == true){                        //If a pulse just happened...
    if ((digitalRead(RotaryPulseIn) == LOW)){  //See if the pin connected to the rotary dial goes low...
      StillOn = false;                         //...if it did, we turn off the "StillOn" flag.
      digitalWrite(StatusLED, HIGH);
      delay(20);
      digitalWrite(StatusLED, LOW);
      delay(40);
      TimeSinceLastPulse = 0;                  //Reset the time since last pulse (not really time... rather a counter for "void loop").
    }
  }

  if (StartTimeSinceLastPulse == true){
    TimeSinceLastPulse++;
  }

  if (TimeSinceLastPulse == 800){    // If standard inter-digit pause (800ms) has elapsed...
    PNumber[k] = n;                  // write the current value of n to the current position (k) in the phone number (PNumber)
    k++;                             // increment to the next position of the phone number.
    FONAserial.print(F("AT+CPTONE="));  // Play DTMF tone over speaker
    if (n != 0)                      // Oh no! We have to convert 0 -> 10 for speaker tones
      FONAserial.println(n);
    else                             // Send 10 instead of 0
      FONAserial.println(10);        // otherwise you get a missing tone when dialling 0.
    delay(20);
    FONAserial.print(F("AT+VTS="));     //Send DTMF tone over network (for menu entries, etc).
    FONAserial.println(n);
    Serial.println(n);
    newrotaryinput = true;
    n = 0;  //reset n
  }
}

void StarPoundRotaryIn(){   //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
  if (digitalRead(RotaryPulseIn) == HIGH){  //If the pin connected to the rotary dial goes high...
    StartTimeSinceLastPulse = true;
    if (StillOn == false){                  //...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
      BarGraphFast(n+1);
      n++;    //...Increment the counter...
      if (n == 10){
        n = 0;
      }
      delay(10);
    }
    StillOn = true;   // ...and turn on the flag that says a pulse just happened.
  }

  if (StillOn == true){     //If a pulse just happened...
    if ((digitalRead(RotaryPulseIn) == LOW)){   //See if the pin connected to the rotary dial goes low...
      StillOn = false;    //...if it did, we turn off the "StillOn" flag.
      digitalWrite(StatusLED, HIGH);
      delay(20);
      digitalWrite(StatusLED, LOW);
      delay(40);
      TimeSinceLastPulse = 0;     //Reset the time since last pulse (not really time... rather a counter for "void loop").
    }
  }

  if (StartTimeSinceLastPulse == true){
    TimeSinceLastPulse++;
  }

  if (TimeSinceLastPulse == 800) {         // If standard inter-digit pause (800ms) has elapsed...
    PNumber[k] = n;    //...and write the current value of n to the current position (k) in the phone number (PNumber)
    k++;      //increment to the next position of the phone number
    if (n == 2){    //Consider this a "*"
      FONAserial.println(F("AT+CPTONE=*"));  //Play DTMF tone over speaker        
      delay(20);
      FONAserial.println(F("AT+VTS=*"));     //Send DTMF tone over network (for menu entries, etc).
    }
    else if (n == 1){  //Consider this a "#"
      FONAserial.println(F("AT+CPTONE=#"));  //Play DTMF tone over speaker        
      delay(20);
      FONAserial.println(F("AT+VTS=#"));     //Send DTMF tone over network (for menu entries, etc).
    }
    n = 0;  //reset n
  }
}


// READ FONA serial:
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


void ClearBuffer(){  //Clear the currently entered phone number
  PNumber[0] = 99;   //Reset the phone number
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
  k = 0;           // Reset the position of the phone number
  BarGraphWipeUp();
}

void ClearBufferSilent(){  //Clear the currently entered phone number
  PNumber[0] = 99;         //Reset the phone number
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
  k = 0;           // Reset the position of the phone number
}

void BarGraphSlow(int level){
  if (level >= 1){
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2){
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3){
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4){
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5){
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6){
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7){
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8){
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9){
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10){
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

void BarGraphFast(int level){
  if (level >= 1){
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2){
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3){
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4){
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5){
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6){
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7){
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8){
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9){
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10){
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

void BarGraphMed(int level){
  Serial.println(level);
  if (level >= 1){
    digitalWrite(BGLED1, HIGH);
  }
  if (level >= 2){
    digitalWrite(BGLED2, HIGH);
  }
  if (level >= 3){
    digitalWrite(BGLED3, HIGH);
  }
  if (level >= 4){
    digitalWrite(BGLED4, HIGH);
  }
  if (level >= 5){
    digitalWrite(BGLED5, HIGH);
  }
  if (level >= 6){
    digitalWrite(BGLED6, HIGH);
  }
  if (level >= 7){
    digitalWrite(BGLED7, HIGH);
  }
  if (level >= 8){
    digitalWrite(BGLED8, HIGH);
  }
  if (level >= 9){
    digitalWrite(BGLED9, HIGH);
  }
  if (level >= 10){
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

void BarGraphWipeUp(){
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

void BarGraphWipeDown(){
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

void checkAlerts() {                                // Check for unsolicited FONA message; incoming call? Caller ID?
  buffer = FONAread(0);                             // get any FONA message in the receive buffer, no waiting
  if (buffer.indexOf("BEGIN") > -1) {               // Did the FONA just receive a call?
    CallOn = true;                                  // We're probably in a call, set the CallOn flag.
    Serial.println(F("INCOMING CALL"));
  }
  if (buffer.indexOf("END:") > -1) {                // Did a call just end?
    CallOn = false;                                 // Reset the CallOn flag.
    Serial.println(F("CALL ENDED"));
  }
  int index1 = buffer.indexOf("+CLIP:");            // Check for Calling Line Identification (CLI) keyword (returns -1 if no Caller ID)
  if (index1 > -1) {                                // Yes, we have a CLI (eg): +CLIP: "02152063113",128,,,,0
    callHour = rtcHour;                             // Save the time
    callMin = rtcMin;
    byte index2 = buffer.indexOf("\"", index1+8);   // Get index of second '"'
    byte index3 = buffer.indexOf("\r", index2);     // Get index of end of line <CR>
    callerID = buffer.substring(index1+8, index2);  // Extract Caller ID. CLI includes a Caller ID if avail and a validity code.
    int IDtype = (buffer.substring(index3 - 1, index3)).toInt();  // Parse CLI validity code, the last digit on line before <CR>.
    switch (IDtype) {
      case 0:                       // 0 = CLI contains a valid Caller ID number.
        // Do nothing unless you want to parse the number, e.g. convert it to a name.
        break;
      case 1:                       // 1 = Caller ID has been withheld by the originator.
        callerID = "Withheld";
        break;
      case 2:                       // 2 = Caller ID is not available due to inter-network problems or limitations.
        callerID = "Not available";
        break;                  
      default:                      // Only ID types 0-2 are valid.
        callerID = "CLI invalid";
    }
  }
}

void displayCID() {                             // Use e-paper partial update to display caller ID
  if (callerID != prevCallerID) {               // Only update display if there is a new Caller ID
    Serial.println(F("Caller ID update"));
    display.setPartialWindow(0, 185, 104, 27);  // Partial update bottom 27 rows of pixels
    display.firstPage();                        // this function is called before every time ePaper is updated.
    do {
      display.fillScreen(GxEPD_WHITE);          // set the background to white (fill the buffer with value for white)
      display.setFont();                        // Back to default font
      display.setCursor(2, 186);
      display.print(F("Caller"));
      if (callerID != "none") {
        display.printf(" at %02d:%02d", callHour, callMin);
      }
      display.setCursor(2, 201); 
      display.print(callerID);
    } while (display.nextPage());
    display.setFullWindow();                   // back to full window
    display.powerOff();
    prevCallerID = callerID;
  }
}

void displayTime() {                            // Use e-paper partial update to display date/time
  getTime();                                    // This updates the RTC time/date global variables
  if (rtcMin != prevRtcMin) {                   // Only update display if minute has changed
    Serial.println(F("Time update"));
    //Serial.print(F("RTC: "));                 // For debugging only: send date/time over serial USB
    //Serial.printf("%d/%d/%d  %02d:%02d\n", rtcDay, rtcMonth, rtcYear+2000, rtcHour, rtcMin);
    display.setPartialWindow(0, 0, 104, 27);    // Partial update top 27 rows of pixels
    display.firstPage();  //this function is called before every time ePaper is updated.
    do {
      display.fillScreen(GxEPD_WHITE);
      display.setFont();                        //Back to default font
      dateStr = dowStr + ' ' + rtcDay + ' ' + monthStr;
      int dateStartX = int((104 - (dateStr.length() * 6)) / 2); // Centre the date horizontally
      display.setCursor(dateStartX, 0);
      display.print(dateStr);
      display.setFont(&FreeMonoBold9pt7b);
      display.setCursor(24, 23);
      display.printf("%02d:%02d", rtcHour, rtcMin);
    } while (display.nextPage());
    display.setFullWindow();                   // back to full window
    display.powerOff();
    prevRtcMin = rtcMin;
  }
}

void getTime() {
  FONAserial.println(F("AT+CCLK?"));               //Get RTC values
  buffer = FONAread(20);                           //Wait up to 20ms for first character
  if ((buffer.indexOf("+CCLK:")) > -1) {
    byte index = buffer.indexOf("+CCLK:");         // Typical RTC msg from FONA: +CCLK: "20/05/25,21:26:08+04"
    buffer = buffer.substring(index+8, index+25);  // Buffer contains RTC in typ format: 20/05/25,21:26:08+04
    //Serial.println(buffer);                      // send the buffer to USB serial if required for debugging
    rtcYear = (buffer.substring(0, 2)).toInt();
    rtcMonth = (buffer.substring(3, 5)).toInt();
    rtcDay = (buffer.substring(6, 8)).toInt();
    rtcHour = (buffer.substring(9, 11)).toInt();
    rtcMin = (buffer.substring(12, 14)).toInt();
    // Calculate day of week (dow) for a given date, Sun=0, Sat=6
    // using Tomohiko Sakamoto's algorithm
    int y = (rtcYear+2000) - (rtcMonth < 3);
    dow = (y + y/4 - y/100 + y/400 + leadingDay[rtcMonth-1] + rtcDay) % 7;
    switch (dow) {
      case 0:
        dowStr = "Sun";
        break;
      case 1:
        dowStr = "Mon";
        break;
      case 2:
        dowStr = "Tue";
        break;
     case 3:
        dowStr = "Wed";
        break;
      case 4:
        dowStr = "Thur";
        break;
      case 5:
        dowStr = "Fri";
        break;
      case 6:
        dowStr = "Sat";
        break;
      default:
        break;
      }
    switch (rtcMonth) {         // Because an old fashioned phone should use words rather than numbers!
      case 1:                   // also some countries use weird day, month number order!
        monthStr = "January";
        break;
      case 2:
        monthStr = "February";
        break;
      case 3:
        monthStr = "March";
        break;
      case 4:
        monthStr = "April";
        break;
     case 5:
        monthStr = "May";
        break;
      case 6:
        monthStr = "June";
        break;
      case 7:
        monthStr = "July";
        break;
      case 8:
        monthStr = "August";
        break;
      case 9:
        monthStr = "September";
        break;
      case 10:
        monthStr = "October";
        break;
      case 11:
        monthStr = "November";
        break;
      case 12:
        monthStr = "December";
        break;
      default:
        break;
      }
  }
  else {
    dowStr = "Invalid";
    monthStr = "time";
  }
}

void BatteryLevel() {
  while (digitalRead(BatteryButton) == LOW) {  // Loop for as long as the battery level button is depressed.
    byte tries = 0;
    bool validMsg = false;
    while (tries < 2 && validMsg == false) {   // 2 tries to get valid battery charge message
      FONAserial.println(F("AT+CBC"));         // The answer is in the form eg: +CBC: 0,100,4.232V
      buffer = FONAread(50);                   // Get response from FONA (wait up to 50ms for the first character).
      if ((buffer.indexOf("+CBC: ")) > -1)     // Was a valid response returned?
        validMsg = true;
      tries++;
    }
    if (validMsg == true) {                    // If valid response was returned in within 2 tries, process it...
      digitalWrite(StatusLED, HIGH);
      buffer = buffer.substring((buffer.indexOf("+CBC:"))+6, (buffer.indexOf("V\r"))+1);  // Extract all battery charge data fields
      Serial.print(F("Battery: "));            // Optional: print battery charge data to USB serial for debugging
      Serial.println(buffer);                  // buffer should now contain battery charge in the form (eg): 0,82,4.050V (eg 82% battery)
      byte index = buffer.indexOf(",");        // find index of first comma, then extract everything between the two commas...
      buffer = buffer.substring(index+1, (buffer.indexOf(",", index+1)));  // buffer now contains string representing % battery charge.
      //Serial.print(F("Battery: "));          // Optional print % battery charge to USB serial for debugging
      //Serial.print(buffer);
      //Serial.println("%");
      BattLevel = buffer.toFloat();            // Convert data type from string to integer
      BarGraphSlow(int(round(BattLevel/10)));  // Scale 0 - 10 and display on LED bargraph
      digitalWrite(StatusLED, LOW);
    } else {
      Serial.println(F("Battery message invalid"));
      delay(200);                              // Don't loop too soon if the buttton is held down
    }
  }
}

void SignalStrength(){
  while (digitalRead(SignalButton) == LOW){    // Loop for as long as the signal strength button is depressed.
    FONAserial.println(F("AT+CSQ"));           // FONA returns signal quality; RSSI (dBm), BER (%)
    buffer = FONAread(50);
    if ((buffer.indexOf("+CSQ:") > -1)) {      // If valid response, extract the 'signal quality' message...
      digitalWrite(StatusLED, HIGH);           // Find of start of signal response, extract chars between start & comma...
      buffer = buffer.substring((buffer.indexOf("+CSQ:"))+6, (buffer.indexOf(",")));
      SigLevel = buffer.toFloat();             // convert data type from string to float
      Serial.print(F("SigLevel code: "));
      Serial.println(buffer);
      int SigBars = int(round((SigLevel * 10 / 31)));  // scale 0 - 31 as 0 - 10
      if (SigBars > 31)                                // 0-31 is valid range, 99 = not known or not detectable 
        SigBars = 0;
      BarGraphSlow(SigBars);
      digitalWrite(StatusLED, LOW);
    }
  }
}

void MakeCall631(){
  ToneReport();
  FONAserial.print(F("ATD631"));
  Serial.print(F("ATD631"));
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
  FONAread(13);                   // Clear the buffer (it will have overflowed due to echoing tone commands)
  FONAserial.println(";");
  Serial.print(";");
  Serial.println(FONAread(13));
  digitalWrite(StatusLED, HIGH);
  delay(500);
  digitalWrite(StatusLED, LOW);
  CallOn = true;
  ClearBufferSilent();
}

void MakeCall(){                  // I really didn't want to use a loop here. Justine's original algorithm is so easy to follow
  ToneReport();                   // but this do... while loop makes variable length dialling much easier.
  CallOn = true;
  byte dd = 0;                    // Dialled Digit index
  FONAserial.print(F("ATD"));
  Serial.print(F("ATD"));
  do {
    FONAserial.print(PNumber[dd]);
    Serial.print(PNumber[dd]);
    dd++;
  } while (PNumber[dd] != 99 && dd < 16);
  FONAread(0);                   // Clear the buffer (it will have overflowed due to echoing tone commands)
  FONAserial.println(";");
  Serial.println(";");
  Serial.println(FONAread(13));
  digitalWrite(StatusLED, HIGH);
  delay(500);
  digitalWrite(StatusLED, LOW);
  CallOn = true;
  ClearBufferSilent();
}

void ToneReport() {                     // Ditto for the speaker tones, a conditional loop is easier here
  byte dd = 0;
  do {
    FONAserial.print(F("AT+CPTONE="));  //Play DTMF tone over speaker
    if (PNumber[dd] != 0)
      FONAserial.println(PNumber[dd]);
    else                                // Send 10 instead of 0 for speaker tones
      FONAserial.println(10);
    BarGraphMed(PNumber[dd]);
    dd++;
  } while (PNumber[dd] != 99 && dd < 16);
}
