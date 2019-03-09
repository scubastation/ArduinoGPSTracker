bool ATchecker(char* test, char CommandAT[20]) {  //Prüffunktion für AT-Kommandos
  int8_t counter, answer;
  unsigned long previous;
  char frame[120];

  while ( mySerial.available() > 0) mySerial.read(); // request Basic string
  //delay(1000); //Kunstpause

  mySerial.println(CommandAT); //Schickt per AT-Befehl die Anfrage

  counter = 0;  //temporäre Variable für die Zeichenanzahl zurücksetzen
  answer = 0;   //temporäre Variable für die Antwort des Moduls zurücksetzen
  memset(frame, '\0', sizeof(frame));    // Initialisieren des String
  previous = millis();
  do {                                    //Diese Schleife startet mit 0 als answer
    if (mySerial.available() != 0)
    { // Wenn das Modul etwas schickt...
      frame[counter] = mySerial.read();     //dann lies es Zeichen für Zeichen in die Variable frame ein
      counter++;                            //und setzte den Zähler nach jedem Zeichen hoch
      if (strstr(frame, "OK") != NULL)     //prüft, ob ein erwartetes OK im String frame enthalten ist
      {
        answer = 1;                         //Wenn das OK gekommen ist setzte die Antwort auf 1, ...
      }
    }
  }
  while ((answer == 0) && ((millis() - previous) < 2000)); //damit die Einleseschleife endet.

  if (strstr(frame, test) == NULL) {       //Prüfen, ob der mitgegebene String enthalten ist
    return false;
  }
  else {
    return true;
  }
}
