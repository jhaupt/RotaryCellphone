
void DisplayContacts(){
  if (PNumber[0] == 1){
    pagenum = 1;
    display.firstPage();                // This function is called before every time ePaper is updated has nothing to do...
    do {                                // with what I call page numbers in this section of the program.
      display.fillScreen(GxEPD_WHITE);  // Set the background to white (fill the buffer with value for white)
      display.setFont(&FreeSans9pt7b);
      display.setCursor(5, 55); 
      display.print(F("Wireless"));
      display.setFont(&FreeSerifItalic9pt7b);
      display.setCursor(30, 75); 
      display.print(F("Electronic"));
      display.setFont(&FreeMono9pt7b);
      display.setCursor(10, 95); 
      display.print(F("Digital"));
      display.setFont(&FreeSerif9pt7b);
      display.setCursor(33, 117); 
      display.print(F("Portable"));
      display.setFont(&FreeSans9pt7b);
      display.setCursor(0, 140); 
      display.print(F("TeLePhOnE"));
      display.setFont();               // Reset to standard built-in (tiny) font
      display.setCursor(2, 155);
      display.print(F("Dial first letter"));
      display.setCursor(2, 165);
      display.print(F("of name to call"));
    } while (display.nextPage());
    forceUpdate();                     // Update Time & Caller ID on next main loop pass
  }
  
  if (PNumber[0] == 2){  //Contacts ABC
    pagenum =2;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS A-C:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("1: Atreides "));    
    } while (display.nextPage());
  }

  if (PNumber[0] == 3){  //Contacts DEF
    pagenum = 3;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS D-F:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("1: Dennis"));
      display.setCursor(0, firstNumbLine + 10);
      display.print(F("2: Duncan"));
      display.setCursor(0, firstNumbLine + 20);
      display.print(F("3: Edward"));
      display.setCursor(0, firstNumbLine + 30);
      display.print(F("4: Entebbe"));
      display.setCursor(0, firstNumbLine + 40);
      display.print(F("5: Frank"));
      display.setCursor(0, firstNumbLine + 50);
      display.print(F("6: Fred"));
    } while (display.nextPage());
  }

  if (PNumber[0] == 4){  //Contacts GHI
    pagenum = 4;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS G-I:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("1: Ghanima"));  
      display.setCursor(0, firstNumbLine + 10);
      display.print(F("2: Irulan"));      
    } while (display.nextPage());
  }

  if (PNumber[0] == 5){  //Contacts JKL
    pagenum = 5;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS J-L:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("1: Jessica"));
      display.setCursor(0, firstNumbLine + 10);
      display.print(F("2: Leto"));
    } while (display.nextPage());
  }

  if (PNumber[0] == 6){  //Contacts MNO
    pagenum = 6;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS M-O:"));
    } while (display.nextPage());
  }

  if (PNumber[0] == 7){  //Contacts PRS
    pagenum = 7;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS P-S:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("1: Paul"));  
      display.setCursor(0, firstNumbLine + 10);
      display.print(F("2: Stilgar"));      
    } while (display.nextPage());
  }

  if (PNumber[0] == 8){  //Contacts TUV
    pagenum =8;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS T-V:"));
      display.setCursor(0, firstNumbLine);
      display.print(F("4: My Voicemail"));  
    } while (display.nextPage());
  }

  if (PNumber[0] == 9){  //Contacts TUV
    pagenum = 9;
    display.firstPage();  //this function is called before every time ePaper is updated. Has nothing to do with what I call page numbers in this section of the program.
    do {
      display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
      display.setFont();  //Back to default font
      display.setCursor(15, 45);
      display.print(F("CONTACTS W-Y:"));
    } while (display.nextPage());
  }
}

void RetrieveContact(){
  if (pagenum == 2){      //Contacts ABC
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 3){      //Contacts DEF
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 3){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 4){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 5){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 6){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 4){      //Contacts GHI
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 5){      //Contacts JKL
    if (PNumber[0] == 1){  
        PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 3){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 6){      //Contacts MNO
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 3){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 7){      //Contacts PRS
    if (PNumber[0] == 1){
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 3){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 8){      //Contacts TUV
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 3){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 4){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
  if (pagenum == 9){      //Contacts WXY
    if (PNumber[0] == 1){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
    if (PNumber[0] == 2){  
      PNumber[0] = 1;
      PNumber[1] = 2;
      PNumber[2] = 3;
      PNumber[3] = 1;
      PNumber[4] = 2;
      PNumber[5] = 3;
      PNumber[6] = 4;
      PNumber[7] = 5;
      PNumber[8] = 6;
      PNumber[9] = 7;
    }
  }
}
