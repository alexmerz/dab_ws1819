/**
 * Besseres Taster-Handling
 */

const int RED = 11;
const int BLUE = 8;
const int TASTER = 2;

// Model
const int OFFL = 0b00; // AUS
const int RL = 0b01; // Rote LED
const int BL = 0b10; // Blaue LED

int ledstate = OFFL;
int tasterstate = 0; 

void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(TASTER, INPUT);
}

void loop()
{
  // Controller
  int t = digitalRead(TASTER);
  if(t == HIGH) {
    tasterstate = 1;
  } else {
    if(tasterstate == 1) { // war Taster vorher gedrueckt?
      tasterstate = 0;
      ledstate++;
      if(ledstate > BL) ledstate = OFFL;       
    }
  }

  // View
  digitalWrite(RED, ledstate&RL);
  digitalWrite(BLUE, ledstate&BL);
} 
