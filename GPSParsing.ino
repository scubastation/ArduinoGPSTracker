void gpsDaten() {     //Holt und parst die GPS/Glossnass Daten
  int8_t counter, answer;
  unsigned long previous;
  char frame[100];
  char UTCdatetime[18];
  char Fixstatus[1];
  char fixmode[1];
  char HDOP[4];
  char PDOP[4];
  char VDOP[4];
  char satellitesinview[2];
  char GNSSsatellitesused[2];
  char GLONASSsatellitesused[2];
  char cn0max[2];
  char HPA[6];
  char VPA[6];

  while ( mySerial.available() > 0) mySerial.read(); // request Basic string
  delay(1000); //Kunstpause

  mySerial.println("AT+CGNSINF"); //Schickt per AT-Befehl die Anfrage

  counter = 0;  //temporäre Variable für die Zeichenanzahl zurücksetzen
  answer = 0;   //temporäre Variable für die Antwort des Moduls zurücksetzen
  memset(frame, '\0', sizeof(frame));    // Initialisieren des String
  previous = millis();
  do {                                    //Diese Schleife startet mit 0 als answer
    if (mySerial.available() != 0)
    { // Wenn das Modul etwas schickt...
      frame[counter] = mySerial.read();   //dann lies es Zeichen für Zeichen in die Variable frame ein
      counter++;                          //und setzte den Zähler nach jedem Zeichen hoch
      if (strstr(frame, "OK") != NULL)    //prüft, ob ein erwartetes OK im String frame enthalten ist
      { //und damit der erwartete Inhalt ist (für GPS-Antwort, die mit OK endet)
        answer = 1;                       //Wenn das OK gekommen ist setzte die Antwort auf 1, ...
      }
    }
  }
  while ((answer == 0) && ((millis() - previous) < 2000)); //damit die Einleseschleife endet.

  // frame[counter - 3] = '\0'; (Packt eine Terminierung am Ende dazu, wofür auch immer)
  Serial.println(frame);  //Debug
  // Parses the string
  strtok(frame, ",");
  strcpy(Fixstatus, strtok(NULL, ",")); // Gets Fix status
  strcpy(UTCdatetime, strtok(NULL, ",")); // Gets UTC date and time
  strcpy(latitude, strtok(NULL, ",")); // Gets latitude
  strcpy(longitude, strtok(NULL, ",")); // Gets longitude
  strcpy(hoehe, strtok(NULL, ",")); // Gets MSL altitude
  strcpy(speedOTG, strtok(NULL, ",")); // Gets speed over ground
  strcpy(kurs, strtok(NULL, ",")); // Gets course over ground
  strcpy(fixmode, strtok(NULL, ",")); // Gets Fix Mode
  strtok(NULL, ",");
  strcpy(HDOP, strtok(NULL, ",")); // Gets HDOP
  strcpy(PDOP, strtok(NULL, ",")); // Gets PDOP
  strcpy(VDOP, strtok(NULL, ",")); // Gets VDOP
  strtok(NULL, ",");
  strcpy(satellitesinview, strtok(NULL, ",")); // Gets GNSS Satellites in View
  strcpy(GNSSsatellitesused, strtok(NULL, ",")); // Gets GNSS Satellites used
  strcpy(GLONASSsatellitesused, strtok(NULL, ",")); // Gets GLONASS Satellites used
  strtok(NULL, ",");
  strcpy(cn0max, strtok(NULL, ",")); // Gets C/N0 max
  strcpy(HPA, strtok(NULL, ",")); // Gets HPA
  strcpy(VPA, strtok(NULL, "\r")); // Gets VPA


  umformatieren(UTCdatetime);  //Zeit umformatieren

  return;
}

void umformatieren(char UTCdatetime[18]) {    //Datum umformatieren

  //Umformatieren: 20190212180038.000 --> 12.02.2019 - 18:00:38
  Datum[0] = UTCdatetime[6];
  Datum[1] = UTCdatetime[7];
  Datum[2] = '.';
  Datum[3] = UTCdatetime[4];
  Datum[4] = UTCdatetime[5];
  Datum[5] = '.';
  Datum[6] = UTCdatetime[0];
  Datum[7] = UTCdatetime[1];
  Datum[8] = UTCdatetime[2];
  Datum[9] = UTCdatetime[3];
  Datum[10] = '\0';

  Uhrzeit[0] = UTCdatetime[8];
  Uhrzeit[1] = UTCdatetime[9];
  Uhrzeit[2] = ':';
  Uhrzeit[3] = UTCdatetime[10];
  Uhrzeit[4] = UTCdatetime[11];
  Uhrzeit[5] = ':';
  Uhrzeit[6] = UTCdatetime[12];
  Uhrzeit[7] = UTCdatetime[13];
  Uhrzeit[8] = '\0';
}
