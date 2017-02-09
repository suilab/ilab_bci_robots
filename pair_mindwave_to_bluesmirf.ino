/*
 * Matthew and Alaina's sketch for pairing BlueSMIRF adapters with Mindwave Mobile devices
 *
 * To use this sketch:
 * 1. Put the MAC address for the Mindwave Mobile in the MINDWAVE_MAC_ADDRESS below. Save the file.
 * 2. Connect RX of your Bluesmirf to TX1 on an Arduino Mega
 * 3. Connect TX of your Bluesmirf to RX1 on an Arduino Mega
 * 4. Connect GND on Bluesmirf to GND on Arduino, likewise +5V
 * 5. Upload this sketch to the Arduino
 * 6. Set your serial monitor speed to 115200
 * 7. Open your serial monitor and follow the instructions
 *
 * TROUBLESHOOTING
 * - Try factory-resetting your BlueSMIRF
 * - Make sure the BlueSMIRF is powered on, and its RX is connected to Arduino TX, and its TX is connected to Arduino RX
 * - Try power cycling the Arduino and then opening the serial monitor
 */
#define MINDWAVE_MAC_ADDRESS "A0E6F8FA0B07"
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
  bluetooth.begin(9600);

  Serial.println("Time to set up a BlueSMIRF adapter to pair to a Mindwave Mobile!");
  Serial.println("(Please see the comments in the Arduino sketch for step by step instructions.");
  Serial.println("Specifically, don't forget to set the MINDWAVE_MAC_ADDRESS!)");
  delay(1000);

  Serial.println("Starting command mode");
  bluetooth.print("$");
  bluetooth.print("$");
  bluetooth.print("$");
  delay(1000);

  Serial.println("Changing pairing code to 0000...");
  bluetooth.println("SP,0000");
  delay(1000);

  Serial.println("Setting board to autoconnect mode...");
  bluetooth.println("SM,3");
  delay(1000);

  Serial.println("Changing authentication mode to no auth...");
  bluetooth.println("SA,0");
  delay(1000);

  Serial.print("Setting Mindwave MAC to ");
  Serial.print(MINDWAVE_MAC_ADDRESS);
  Serial.println("...");
  bluetooth.print("SR,");
  bluetooth.println(MINDWAVE_MAC_ADDRESS);
  delay(1000);

  Serial.println("Setting baud rate to 57.6k...");
  bluetooth.println("SU,57.6");
  delay(1000);

  Serial.println("Exiting command mode...");
  bluetooth.println("---");

  Serial.println("Done configuring! Next steps to confirm pairing:");
  Serial.println("1. Power off the BlueSMIRF adapter");
  Serial.println("2. Power on the Mindwave Mobile headset");
  Serial.println("3. Power on the BlueSMIRF adapter");
  Serial.println("4. After a few seconds, the devices should pair!");
}

void loop() {
 // do nothing; it was all done in setup()
}
