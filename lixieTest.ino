#include "Lixie.h" // Include Lixie Library

#define DATA_PIN   13
#define NUM_LIXIES 2
Lixie lix(DATA_PIN, NUM_LIXIES); // Set class name to "lix" and define your Lixie setup

uint16_t count = 0; // Use this number to count up
String a;
char input[3];

void setup() {
  lix.begin();                   // Initialize LEDs
  lix.nixie_mode(true);
  lix.brightness(128);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  Serial.println("Enter seconds:");
while(Serial.available()) {

a= Serial.readString();// read the incoming data as string

Serial.println(a);

}
  Serial.println(a);
  a.toCharArray(input,3);
  lix.write(input);             // Write the count to the displays
  delay(100);

}
