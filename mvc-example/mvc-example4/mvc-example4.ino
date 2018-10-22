/**
 * Beispiel mit 1 Zustands-Variablen mit binären Status
 */

const int RED = 11;
const int BLUE = 8;
const int TASTER = 2;
const long PERIOD = 100000;

// Model
const int OFFL = 0b00; // AUS
const int RL = 0b01; // Rote LED
const int BL = 0b10; // Blaue LED

int state = OFFL;
  
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
      state++;
      if(state > BL) state = OFFL;
    } 
    timestamp = micros() + PERIOD;
  }

  // View
  digitalWrite(RED, state&RL);
  digitalWrite(BLUE, state&BL);
} 
