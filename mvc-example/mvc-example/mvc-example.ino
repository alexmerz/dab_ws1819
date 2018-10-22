/**
 * Beispiel mit 2 Variablen
 */

const int RED = 11;
const int BLUE = 8;
const int TASTER = 2;
const long PERIOD = 100000;

// Model
int red = LOW;
int blue = LOW;
  
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
        red = HIGH;
        blue = LOW;
      } else if(digitalRead(RED) == HIGH && digitalRead(BLUE) == LOW) {
        red = LOW;
        blue = HIGH;
      } else if(digitalRead(RED) == LOW && digitalRead(BLUE) == HIGH) {
        red = LOW;
        blue = LOW;
      }       
    } 
    timestamp = micros() + PERIOD;
  }

  // View
  digitalWrite(RED, red);
  digitalWrite(BLUE, blue);
  
} 
