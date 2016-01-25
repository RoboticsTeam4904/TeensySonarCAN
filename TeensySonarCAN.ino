#define trigPin 18
#define echoPin 19

#include <FlexCAN.h>
#include <TeensyCANBase.h>


int led = 13;
TeensyCANBase can(0x222);

void setup(void) {
  can.begin();

  pinMode(led, OUTPUT);

  delay(1000);
  Serial.println(F("Teensy 3.x CAN Sonar Sensor"));

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

int duration, distance;

void readSonar() {

  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 500000);
  distance = (duration / 2.0) / 2.91;
  if (distance < 300) {  // This is where the LED On/Off happens
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
  if (distance >= 4000 || distance <= 50) {
    Serial.println("Out of range");
    distance = 0;
  }
  else {
    Serial.print(distance);
    Serial.println(" mm");
  }
}
// -------------------------------------------------------------
void loop(void) {
  if (can.available() > 0) {
    uint8_t * txdata = (uint8_t *) malloc(8);
    if (can.read(txdata) == 0) {
      txdata[0] = distance & 0xff;
      txdata[1] = (distance >> 8) & 0xff;

      for (int i = 2; i < 8; i++) {
        txdata[i] = 0;
      }

      can.write(txdata);
    }
    delete txdata;
  }

  readSonar();
}

