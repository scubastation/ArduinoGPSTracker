
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
