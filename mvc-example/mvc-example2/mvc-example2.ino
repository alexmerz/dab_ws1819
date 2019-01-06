/**
 * Beispiel mit Struct
 */

const int RED = 11;
const int BLUE = 8;
const int TASTER = 2;
const long PERIOD = 100000;

// Model
struct Leds {
  int red;
  int blue;
};

struct Leds leds = {LOW, LOW};
  
long timestamp = 0;

void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(TASTER, INPUT);
}

void loop()
{
  // Controller
  if(micros() > timestamp + PERIOD) {
    if(digitalRead(TASTER) == HIGH) {
      if(digitalRead(RED) == LOW && digitalRead(BLUE) == LOW) {
        leds.red = HIGH;
        leds.blue = LOW; 
      } else if(digitalRead(RED) == HIGH && digitalRead(BLUE) == LOW) {
        leds.red = LOW;
        leds.blue = HIGH;
      } else if(digitalRead(RED) == LOW && digitalRead(BLUE) == HIGH) {
        leds.red = LOW;
        leds.blue = LOW;
      }       
    } 
    timestamp = micros() + PERIOD;
  }

  // View
  digitalWrite(RED, leds.red);
  digitalWrite(BLUE, leds.blue);
  
} 
