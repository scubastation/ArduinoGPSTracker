void generateMessage(){ 

sprintf(smsMessage, "%s  - %s\nLatitude : %s\nLongitude : %s\nSpeed : %s kph\nhttp://maps.google.com/maps?q=%s,%s\n", Datum, Uhrzeit, latitude, longitude, speedOTG, latitude, longitude);
  Serial.println("Generierte Nachricht: ");
  Serial.println(smsMessage);

return; 
}
