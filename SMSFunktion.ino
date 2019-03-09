
void sendSMS(char smsMessage[160], char phoneNumber[13]) {
  char sendstring[200] = "";

  ATchecker("OK", "AT+CMGF=1");  //SMS Textmodus setzen
  Serial.println("SMS Textmodus gesetzt");

  strcat(sendstring, "AT+CMGS=\"");
  strcat(sendstring, phoneNumber);
  strcat(sendstring, "\"\r");

  strcat(sendstring, smsMessage);
  strcat(sendstring, "\r");

  Serial.println("sendString ist: ");
  Serial.println(sendstring);
  mySerial.print(sendstring);
  delay(500);
  mySerial.write(0x1A);  //Equivalent to sending Ctrl+Z

}
