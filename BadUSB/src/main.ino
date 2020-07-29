#include <Wire.h>
#include <Keyboard.h>

#define ADDR 4

String msg = "";
char command;

void setup() {
  Wire.begin(ADDR);             // join i2c bus with address
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  Keyboard.begin();
}

void loop() {
  delay(1);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

void receiveEvent(int Length) {
  if (Length == 4){
    while(2 < Wire.available()) { // loop through all but the last
      char c = Wire.read(); // receive byte as a character
      msg += c;
     // Serial.println(msg);
    }
    if(msg == "p:") {
      command = Wire.read();
      if(Wire.read() == ';') {
        Serial.print("Command: ");
        Serial.println(command);
        Keyboard.write(command);
      } else {
        Serial.println("Error ):");
      }
    } else {
      Serial.println("Error ):");
    }
  } else {
    Serial.println("Error ):");
  }
  msg = "";
}
