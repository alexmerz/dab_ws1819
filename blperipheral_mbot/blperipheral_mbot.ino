#include <Arduino.h>
// #include <SoftwareSerial.h>
#include <MeMCore.h>

/* structures and defines */
const unsigned long ULTRASONIC_TIME = 1000;
const unsigned long CONNECTALIVE = 5000;

const int COMMANDS = 5;
const char *Commands[COMMANDS] = {"ID", "FORWARD", "CLEAR", "RGB", "DIST"};
const int MAXCOMMANDLENGTH = 128;
const int MAXELEMLENGTH = 32;
typedef struct {
  double ultrasonic;
} Sensor;

typedef struct {
  int red;
  int green;
  int blue;
} RGB;

typedef struct {
  RGB rgb0;
  RGB rgb1;
  short motor0;
  short motor1;
} Actor;

typedef struct {
  unsigned long ultrasonic;
  unsigned long motor0;
  unsigned long motor1;
  unsigned long connectalive;
} Timer;

/* variables */
Sensor sensor = {0};
Timer timer = {0, 0, 0};
Actor actor;
char activeCommand[4][MAXELEMLENGTH];

MeUltrasonicSensor ultrasonic(PORT_3);
MeDCMotor motor0(M1);
MeDCMotor motor1(M2);
MeRGBLed led(7, 2);

char readbuffer[128];
int bindex = 0;

void setup() {
  setupActor();  
  clearReadbuffer();
  Serial.begin(115200);    
  Serial.println("READY");
}

void loop() {  
  unsigned long timestamp = millis();
  int readdata = 0; 
  
  isConnectAlive();
  
  if(timer.ultrasonic + ULTRASONIC_TIME > timestamp) {
    sensor.ultrasonic = ultrasonic.distanceCm();
    timer.ultrasonic = timestamp;    
  }

  riskDetection();
  if(Serial.available()) {
    int s = Serial.read();
    while(-1 != s) {
      if(10 == s) {
        analyseCommand(readbuffer);
        clearReadbuffer();
        break;
      } else {
        if(bindex < MAXCOMMANDLENGTH) {
          readbuffer[bindex] = s;
          bindex++;
        }
      }
      s = Serial.read();
    }    
  }
  processCommand();
  processActor();
  processTimer();
}

void clearReadbuffer() {
  memset(readbuffer, 0, sizeof(readbuffer));
  bindex = 0;
}

void clearCommand() {
  memset(activeCommand[0], 0, sizeof(activeCommand[0]));
  memset(activeCommand[1], 0, sizeof(activeCommand[1]));
  memset(activeCommand[2], 0, sizeof(activeCommand[2]));  
  memset(activeCommand[3], 0, sizeof(activeCommand[3]));
}


void riskDetection() {
  if(sensor.ultrasonic < 20) {
    Serial.println("ALERT OBSTACLE");
  }
}

void processTimer() {
  unsigned long ts = millis();
  if(ts > timer.motor0) {
    actor.motor0 = 0;
  }
  if(ts > timer.motor1) {
    actor.motor1 = 0;
  }  
}

void processCommand() {  
  if(0 == activeCommand[0][0]) {
    return;
  }
  String command = String(activeCommand[0]);  
  if(command.equals("ID")) {
    Serial.print("Hallo\n");
    Serial.print("OK\n");
    clearCommand();
    return;
  } else if(command.equals("FORWARD")) {
    forwardCommand();
    clearCommand();
    return;
  } else if(command.equals("RGB")) {
    rgbCommand();
    clearCommand();
    return;
  } else if(command.equals("DIST")) {
    distCommand();
    clearCommand();
    return;
  } 
}

void splitBuffer() {
  int i = 0;
  int j = 0;
  int k = 0;
  for(; j < 4; j++) {
    k = 0;
    for(; i < MAXELEMLENGTH; i++) { // command
      if(32 == readbuffer[i]) { // next element on space
        i++;
        break;
      } else if(0 == readbuffer[i]) { // end of string
        return;
      } else {
        activeCommand[j][k] = readbuffer[i];  
        k++;
      } 
    }    
  }
}

void analyseCommand(char *readbuffer) {
  clearCommand();
  splitBuffer();

  if(0 == activeCommand[0][0]) { // empty call
    clearCommand();
    clearReadbuffer();
    return;
  }  
  
  String command = String(activeCommand[0]);
  boolean allowed = false;
  int i;
  for(i = 0; i < COMMANDS; i++) {    
    if(true == command.equals(Commands[i])) {      
      allowed = true;
      break;
    }
  }

  if(false == allowed) {    
    Serial.print("NOK UC\n");
    return;
  } 
  // Special handling for CLEAR because it clears 
  if(true == command.equals("CLEAR")) {      
    Serial.print("OK\n");          
  }
  connectAlive();  
}

boolean isConnectAlive() {

  unsigned long ts = millis();
  if(ts < timer.connectalive) {
    return true;
  }
  Serial.end();  
  setupActor();
  Serial.begin(115200);    

  return false;

}

void connectAlive() {
  unsigned long ts = millis();
  timer.connectalive = ts + CONNECTALIVE;
}

/* Actor related functions */

void setupActor() {
  setRGB(0,0,0);
  actor.motor0 = 0;
  actor.motor1 = 0;  
}

void setRGB(int r, int g, int b) {
  actor.rgb0.red = r;
  actor.rgb1.red = r;
  actor.rgb0.green = g;
  actor.rgb1.green = g;
  actor.rgb0.blue = b;
  actor.rgb1.blue = b;    
}

void processActor() {  
  
  if(actor.motor0 == 0) {
    motor0.stop();
  } else {
    motor0.run(actor.motor0);
  }
  if(actor.motor1 == 0) {
    motor1.stop();
  } else {
    motor1.run(actor.motor1);
  } 
  
  led.setColor(0, actor.rgb0.red, actor.rgb0.green, actor.rgb0.blue);
  led.setColor(1, actor.rgb1.red, actor.rgb1.green, actor.rgb1.blue);  
  led.show();
}

/* commands */

void distCommand() {
  Serial.println(sensor.ultrasonic);
  Serial.println("OK");
}

// RGB r g b
void rgbCommand() {
  int r = 0;
  int g = 0;
  int b = 0;

  if(0 != activeCommand[1][0] && (0 == activeCommand[2][0] && 0 == activeCommand[2][0])) {
    r = atoi(activeCommand[1]);
    g = r;
    b = r;
  } else {
    r = atoi(activeCommand[1]);
    g = atoi(activeCommand[2]);
    b = atoi(activeCommand[3]);
  }

  setRGB(r, g, b);
  Serial.print("OK\n");
  return;
}

// expects duration and speed as parameters
void forwardCommand() {
  if(sensor.ultrasonic < 5) {
    Serial.println("NOK OBSTACLE");
    return;
  }
  
  int speed = 100;
  unsigned long ts = millis();
  unsigned long duration = 1000;
  
  if(0 != activeCommand[1][0]) {
    duration = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2][0]) {
    speed = atoi(activeCommand[2]);
  }

  actor.motor0 = -speed;
  actor.motor1 = speed;    
  timer.motor0 = ts + duration;
  timer.motor1 = ts + duration;
  Serial.print("OK\n");
  return;
}


