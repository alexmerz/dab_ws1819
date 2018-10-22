/**
 * Beispiel mit 1 Zustands-Variablen
 */

const int RED = 11;
const int BLUE = 8;
const int TASTER = 2;
const long PERIOD = 100000;

// Model
const int OFFL = 0; // AUS
const int RL = 1; // Rote LED
const int BL = 2; // Blaue LED

int state = OFFL;
  
long timestamp = 0;

void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(TASTER, INPUT);
}

void leds(int red, int blue) {
  digitalWrite(RED, red);
  digitalWrite(BLUE, blue);
}

void loop()
{
  // Controller
  if(micros() > timestamp + PERIOD) {
    if(digitalRead(TASTER) == HIGH) {
      state++;
      if(state > BL) state = OFFL;
    } 
    timestamp = micros() + PERIOD;
  }

  // View
  switch(state) {
    case OFFL:
      leds(LOW, LOW);
      break;
    case RL:
      leds(HIGH, LOW);
      break;
    case BL:
      leds(LOW, HIGH);
      break;    
  }
  
} 
