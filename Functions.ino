void RotaryIn(){   //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
	if (digitalRead(RotaryPulseIn) == HIGH){    //If the pin connected to the rotary dial goes high...
		StartTimeSinceLastPulse = true;
		if (StillOn == false){     //...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
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

	if (TimeSinceLastPulse == 1000){    //If a long enough time has lapsed since the last pulse from the rotary dial, assume the dial has stopped rotating...
		PNumber[k] = n;    //...and write the current value of n to the current position (k) in the phone number (PNumber)
		k++;      //increment to the next position of the phone number
		FONAserial.print("AT+CPTONE=");	//Play DTMF tone over speaker
		FONAserial.println(n);
		delay(20);
		FONAserial.print("AT+VTS=");	//Send DTMF tone over network (for menu entries, etc).
		FONAserial.println(n);
		newrotaryinput = true;
		n = 0;  //reset n
	}
}

void StarPoundRotaryIn(){   //Listen for pulses from the rotary dial. NO LOOP here. This runs once per iteration of "void loop", so the timing of other things in the main loop will affect this.
	if (digitalRead(RotaryPulseIn) == HIGH){    //If the pin connected to the rotary dial goes high...
		StartTimeSinceLastPulse = true;
		if (StillOn == false){     //...and if we're not still in the same (continuous) pulse from the previous iteration of void loop()...
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

	if (TimeSinceLastPulse == 1000){    //If a long enough time has lapsed since the last pulse from the rotary dial, assume the dial has stopped rotating...
		PNumber[k] = n;    //...and write the current value of n to the current position (k) in the phone number (PNumber)
		k++;      //increment to the next position of the phone number
		if (n == 2){		//Consider this a "*"
			FONAserial.println("AT+CPTONE=*");	//Play DTMF tone over speaker				
			delay(20);
			FONAserial.println("AT+VTS=*");	//Send DTMF tone over network (for menu entries, etc).
		}
		else if (n == 1){	//Consider this a "#"
			FONAserial.println("AT+CPTONE=#");	//Play DTMF tone over speaker				
			delay(20);
			FONAserial.println("AT+VTS=#");	//Send DTMF tone over network (for menu entries, etc).
		}
		n = 0;  //reset n
	}
}


// READ FONA serial:
// Return all serial data available from the FONA as a String object. The argument is the time to wait (ms) for 
// the first character, can be zero or 13ms upwards (the battery would go flat before max delay). A small wait is 
// useful where there might be a delay in FONA response. Note the SoftwareSerial buffer length is 63 characters.
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

void ClearBufferSilent(){  //Clear the currently entered phone number
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

void BarGraphSlow(int level){
  Serial.print(F("Bargraph: "));
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

void displayTime() {           //  Use e-paper partial update to display date/time
  getTime();
  Serial.print("RTC: ");  // Send date/time over serial USB for debugging
  Serial.printf("%d/%d/%d  %02d:%02d\n", rtcDay, rtcMonth, rtcYear+2000, rtcHour, rtcMin);
  display.setPartialWindow(0, 185, 104, 26);    // Partial update of bottom 26 rows of pixels
  display.firstPage();  //this function is called before every time ePaper is updated.
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont();             //Back to default font
    display.setCursor(5, 185);
    display.print(dowStr);
    display.printf(" %d ", rtcDay);
    display.print(monthStr);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(24, 208);
    display.printf("%02d:%02d", rtcHour, rtcMin);
  } while (display.nextPage());
  display.setFullWindow();        // back to full window
  display.powerOff();
}

void getTime() {
  FONAserial.println("AT+CCLK?");          //Get RTC values
  buffer = FONAread(50);                  //Wait up to 50ms for first character
  if ((buffer.indexOf("+CCLK:")) > -1) {
    byte index = buffer.indexOf("+CCLK:");         // Typical RTC msg from FONA: +CCLK: "20/05/25,21:26:08+04"
    buffer = buffer.substring(index+8, index+25);  // Buffer contains RTC in typ format: 20/05/25,21:26:08+04
    //Serial.println(buffer);                    // send the buffer to USB serial if required for debugging
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
        dowStr = "Thu";
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
      FONAserial.println("AT+CBC");            // The answer is in the form eg: +CBC: 0,100,4.232V
      buffer = FONAread(50);                   // Get response from FONA (wait up to 50ms for the first character).
      if ((buffer.indexOf("+CBC: ")) > -1) {   // Was a valid response returned?
        validMsg = true;
        tries++;
      }
    }
    if (validMsg == true) {                    // If valid response was returned in within 2 tries, process it...
      digitalWrite(StatusLED, HIGH);
      buffer = buffer.substring((buffer.indexOf("+CBC:"))+6, (buffer.indexOf("V\r"))+1);  // Extract all battery charge data fields
      Serial.print("Battery: ");               // Optional: print battery charge data to USB serial for debugging
      Serial.println(buffer);                  // buffer should now contain battery charge in the form (eg): 0,82,4.050V (eg 82% battery)
      byte index = buffer.indexOf(",");        // find index of first comma, then extract everything between the two commas...
      buffer = buffer.substring(index+1, (buffer.indexOf(",", index+1)));  // buffer now contains string representing % battery charge.
      //Serial.print("Battery: ");             // Optional print % battery charge to USB serial for debugging
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
		FONAserial.println("AT+CSQ");	          	 // FONA returns signal quality; RSSI (dBm), BER (%)
		buffer = FONAread(50);
		if ((buffer.indexOf("+CSQ:")) > -1) {      // If valid response, extract the 'signal quality' message...
      digitalWrite(StatusLED, HIGH);           // Find of start of signal response, extract chars between start & comma...
			buffer = buffer.substring((buffer.indexOf("+CSQ:"))+6, (buffer.indexOf(",")));
			SigLevel = buffer.toFloat();		         // convert data type from string to float
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
	Serial.println(FONAread(13));
	digitalWrite(StatusLED, HIGH);
	delay(500);
	digitalWrite(StatusLED, LOW);
	CallOn = true;
	ClearBufferSilent();
}

void MakeCall(){
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
	Serial.println(FONAread(13));
	digitalWrite(StatusLED, HIGH);
	delay(500);
	digitalWrite(StatusLED, LOW);
	CallOn = true;
	ClearBufferSilent();
}

void ToneReport(){
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
