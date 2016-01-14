
#define trigPin 18
#define echoPin 19

#include <FlexCAN.h>



int led = 13;
FlexCAN CANbus(1000000);
static CAN_message_t msg, rxmsg;
static uint8_t hex[17] = "0123456789abcdef";

int txCount, rxCount;
unsigned int txTimer, rxTimer;


// -------------------------------------------------------------
static void hexDump(uint8_t dumpLen, uint8_t *bytePtr)
{
  uint8_t working;
  while ( dumpLen-- ) {
    working = *bytePtr++;
    Serial.write( hex[ working >> 4 ] );
    Serial.write( hex[ working & 15 ] );
  }
}


CAN_filter_t filter;
// -------------------------------------------------------------
void setup(void)
{
  filter.id = 0x222;
  CANbus.begin(filter);
  //CANbus.setFilter(0x222, 0);
  pinMode(led, OUTPUT);

  delay(1000);
  Serial.println(F("Hello Teensy 3.1 CAN Test."));

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}


int duration, distance;



// -------------------------------------------------------------
void loop(void)
{
  while ( CANbus.read(rxmsg) ) {
    if (rxmsg.id == 0x222) {
      Serial.print("T: ");
      Serial.print(millis());
      Serial.print(" ID: 0x");
      hexDump( sizeof(rxmsg.id), (uint8_t *)&rxmsg.id );
      Serial.print(" MSG: 0x");
      hexDump( sizeof(rxmsg.buf), (uint8_t *)&rxmsg.buf );
      Serial.print(" ASCII: ");
      for ( int i = 0; i < sizeof(rxmsg.buf); i++) {
        Serial.write(rxmsg.buf[i]);
      }
      Serial.println();

      msg.len = 8;
      msg.id = 0x222;

      msg.buf[0] = distance & 0xff;
      msg.buf[1] = (distance >> 8) & 0xff;
      

      CANbus.write(msg);
    }
  }

  readSonar();
}

void readSonar() {

  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 500000);
  distance = (duration / 2.0) / 2.91;
  if (distance < 300) {  // This is where the LED On/Off happens
    digitalWrite(led, HIGH); // When the Red condition is met, the Green LED should turn off
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
