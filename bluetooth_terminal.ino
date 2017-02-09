/*
 * A serial terminal for a Bluetooth adapter connected to Serial1
 * RX of BT adapter to TX1 of Arduino Mega
 * TX of BT adapter to RX1 of Arduino Mega
 * Open serial monitor at 115200 and communicate with the BT adepter
 *
 * Remember, to get into command mode you have to send "$$$" with no
 * carriage return or linefeed.
 */
#define bluetooth Serial1
void setup()
{
  Serial.begin(115200);  // Begin the serial monitor at 9600bp
  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  delay(100);
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);
  Serial.println("I'm here!");
}
void loop() { // run over and over
  if (bluetooth.available()) {
    Serial.write((char)bluetooth.read());
  }
  if (Serial.available()) {
    bluetooth.write((char)Serial.read());
  }
}

