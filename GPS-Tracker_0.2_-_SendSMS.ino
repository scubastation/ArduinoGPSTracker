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
void LCDAnzeige();
void monitorAnzeige();
void sendSMS(char smsMessage[160], char phoneNumber[13]);
void generateMessage();
//Funktionen Ende

//globale Variablen
const char* telefonnummer="01717507988";
char latitude[12]; //= "52.601745";
char longitude[12]; //="13.342538";
char hoehe[8];
char speedOTG[6]; //="0.04";
char kurs[6];
char Uhrzeit[9]; //= "11:53:52";
char Datum[11]; //="09.03.2019";
char smsMessage[160];
//globale Variablen Ende

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.begin();
  lcd.clear();
  lcd.home();
  lcd.print("LCD angeschaltet");  //Reine Debig-Info
  Serial.println("LCD an");
  delay(5000);

  //Prüfen, ob sich die SIM-Karte ins Netz gebucht hat
  while (ATchecker(",1", "AT+CREG?") == false)
  {
    lcd.setCursor(0, 1);
    lcd.print("SIM kein Signal");
    Serial.println("Sim kein Signal");
    delay(5000);
  }
  lcd.setCursor(0, 1);
  lcd.print("SIM eingebucht     ");
  Serial.println("SIM eingebucht");
  delay(3000);
  lcd.clear();
}

void loop() {
  gpsBereitschaftPruefen();
  gpsDaten();
  //LCDAnzeige();
  monitorAnzeige();
  generateMessage();
  Serial.println("Im Hauptprogramm: ");
  Serial.println(smsMessage);
  sendSMS(smsMessage, telefonnummer);
  
  delay (10000);
  exit(0);  //Wenn für debug  nur eine SMS gesendet werden soll.
  
}
