/*
  LCD-Display Anschluss:
  SDA --> A4
  SCL --> A5

  SIM808 Anschluß:
  TX --> Digital 3 (RX am Board)
  RX --> Digital 2 (TX am Board)

  Beim Verwenden eines BK-808 v2 auf ausreichend Stromversorgung achten (Ampere).
  Autonom betreiben: Batterie ans 808 und Vbat an Vin des Arduino um ihn mit Strom zu versorgen
*/

#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>        //  LiquidCrystal_I2C Bibliothek installiren, falls noch nicht vorhanden
LiquidCrystal_I2C lcd(0x27, 20, 4);   // Hier wird festgelegt um was für einen Display es sich handelt.
// In diesem Fall einer mit 20 Zeichen in 4 Zeilen auf Adresse 0x27.

#define PIN_RX 3   //ans TX des 808-Boards
#define PIN_TX 2   //ans RX des 808-Boards
SoftwareSerial mySerial(PIN_RX, PIN_TX);

//Funktionen
bool ATchecker(char* check, char CommandAT[20]); //Die Funktion prüft, ob ein abgearbeitetes AT-Kommando einen Gewünschten Wert enthält
void gpsDaten();
void umformatieren(char UTCdatetime[18]);
void clearline();
void LCDAnzeige();
void monitorAnzeige();
//Funktionen Ende

//globale Variablen
char latitude[12];
char longitude[12];
char hoehe[8];
char speedOTG[6];
char kurs[6];
char Uhrzeit[9];
char Datum[11];
//globale Variablen Ende

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.begin();
  lcd.clear();
  lcd.home();
  lcd.print("LCD angeschaltet");  //Reine Debig-Info
  delay(5000);

  //Prüfen, ob sich die SIM-Karte ins Netz gebucht hat
  while (ATchecker(",1", "AT+CREG?") == false)
  {
    lcd.setCursor(0, 1);
    lcd.print("SIM kein Signal");
    delay(5000);
  }
  lcd.setCursor(0, 1);
  lcd.print("SIM eingebucht     ");
  delay(3000);
  lcd.clear();
}

void loop() {
  gpsBereitschaftPruefen();
  gpsDaten();
  LCDAnzeige();
  monitorAnzeige();
  delay (10000);
}

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

void LCDAnzeige() {  //Ausgabe der Daten auf LCD
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.setCursor(0, 0);
  lcd.print("UTC: ");
  lcd.print(Uhrzeit);

  lcd.setCursor(0, 1);
  lcd.setCursor(0, 1);
  lcd.print("Lat: ");
  lcd.print(latitude);

  lcd.setCursor(0, 2);
  lcd.setCursor(0, 2);
  lcd.print("Lon: ");
  lcd.print(longitude);

  lcd.setCursor(0, 3);
  lcd.setCursor(0, 3);
  lcd.print("Speed: ");
  lcd.print(speedOTG);
}

void monitorAnzeige() {   //Ausgabe der Daten auf dem seriellen Monitor

  Serial.print("UTC-Zeit: ");
  Serial.println(Uhrzeit);
  Serial.print("Datum: ");
  Serial.println(Datum);

  Serial.print("Latitude: ");
  Serial.println(latitude);
  Serial.print("Longitude: ");
  Serial.println(longitude);

  Serial.print("Speed: ");
  Serial.println(speedOTG);
}

bool gpsBereitschaftPruefen() {   //Prüft die Bereitschaft des GPS
  lcd.clear();

  ///Sicherstellen, dass GPS eingeschaltet ist
  while (ATchecker("OK", "AT+CGPSPWR=1") == false)
  {
    lcd.setCursor(0, 0);
    lcd.print("GPS noch nicht an");
    delay(5000);
  }
  lcd.setCursor(0, 0);
  lcd.print("GPS hat Strom      ");

  delay(2000);

  //GPS-Status auf 3D-Fix checken
  while (ATchecker("3D", "AT+CGPSSTATUS?") == false)
  {
    lcd.setCursor(0, 1);
    lcd.print("GPS kein Fix");
    delay(5000);
  };
  lcd.setCursor(0, 1);
  lcd.print("GPS 3D-Fix     ");

  //GPS Satz erhalten?
  while (ATchecker("1,", "AT+CGNSINF") == false)
  {
    lcd.setCursor(0, 2);
    lcd.print("Kein GPS-Satz");
  };
  lcd.setCursor(0, 2);
  lcd.print("GPS-Satz erhalten   ");

  return true;
}
