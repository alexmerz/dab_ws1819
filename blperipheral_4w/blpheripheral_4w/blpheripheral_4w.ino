#include <MeMegaPi.h>
#include <Wire.h>

const int FRONT = 0x01;
const int BACK = 0x02;
const int RIGHT = SLOT2;
const int LEFT = SLOT1;

const unsigned long CONNECTALIVE = 5000;

const int COMMANDS = 8;
const char *Commands[COMMANDS] = {"ID", "FORWARD", "BACK", "RCIRCLE", "LCIRCLE", "LEFT", "RIGHT", "CLEAR"};
const int MAXCOMMANDLENGTH = 128;
const int MAXELEMLENGTH = 32;

typedef struct {
  short motorF_L;
  short motorB_L;
  short motorF_R;  
  short motorB_R;  
} Actor;

typedef struct {
  unsigned long motorF_L;
  unsigned long motorB_L;
  unsigned long motorF_R;
  unsigned long motorB_R;
  unsigned long connectalive;
} Timer;

Timer timer = {0, 0, 0, 0, 0};
Actor actor = {0, 0 ,0 ,0};
char activeCommand[4][MAXELEMLENGTH];

char readbuffer0[128];
int bindex0 = 0;
char readbuffer3[128];
int bindex3 = 0;

MeEncoderNew motorFRONT_LEFT(FRONT, LEFT);
MeEncoderNew motorBACK_LEFT(BACK, LEFT);
MeEncoderNew motorFRONT_RIGHT(FRONT, RIGHT);
MeEncoderNew motorBACK_RIGHT(BACK, RIGHT);

void setup() {
  motorFRONT_LEFT.begin();
  motorFRONT_LEFT.reset();
  motorFRONT_RIGHT.begin();
  motorFRONT_RIGHT.reset();
  motorBACK_LEFT.begin();
  motorBACK_LEFT.reset();
  motorBACK_RIGHT.begin();
  motorBACK_RIGHT.reset();
  
  setupActor();  
  
  Serial.begin(115200);
  Serial.println("READY");
  Serial3.begin(115200);  
  Serial3.println("READY");
}

void setupActor() {
  actor.motorF_L = 0;
  actor.motorB_L = 0;  
  actor.motorF_R = 0;
  actor.motorB_R = 0;
}

void loop() { 
  //isConnectAlive();
  checkForSerialCommand();
  checkForBTCommand();
  
  processCommand();
  processTimer();
  processActor();  
}

void checkForBTCommand() {
  if(Serial3.available()) {
    int s = Serial3.read();    
    while(-1 != s) {
      if(10 == s) {
        if(bindex3 < MAXCOMMANDLENGTH) {  // make sure, string is \0 terminated        
            readbuffer3[bindex3] = 0;
            bindex3++;
        }
        analyseCommand(readbuffer3);
        clearReadbuffer3();
        break;
      } else {
        if(bindex3 < MAXCOMMANDLENGTH) {          
          readbuffer3[bindex3] = s;
          bindex3++;
        }
      }
      s = Serial3.read();
    }    
  }
}

void checkForSerialCommand() {
  if(Serial.available()) {    
    int s = Serial.read();    
    while(-1 != s) {
      if(10 == s) {
        if(bindex0 < MAXCOMMANDLENGTH) {  // make sure, string is \0 terminated        
            readbuffer0[bindex0] = 0;
            bindex0++;
        }
        analyseCommand(readbuffer0);
        clearReadbuffer0();
        break;
      } else {
        if(bindex0 < MAXCOMMANDLENGTH) {          
          readbuffer0[bindex0] = s;
          bindex0++;
        }
      }
      s = Serial.read();
    }    
  }
}

void clearReadbuffer0() {
  memset(readbuffer0, 0, sizeof(readbuffer0));
  bindex0 = 0;  
}

void clearReadbuffer3() {
  memset(readbuffer3, 0, sizeof(readbuffer3));
  bindex3 = 0;  
}

void clearCommand() {
  memset(activeCommand[0], 0, sizeof(activeCommand[0]));
  memset(activeCommand[1], 0, sizeof(activeCommand[1]));
  memset(activeCommand[2], 0, sizeof(activeCommand[2]));  
  memset(activeCommand[3], 0, sizeof(activeCommand[3]));
}

void splitBuffer(char *readbuffer) {  
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
  splitBuffer(readbuffer);

  if(0 == activeCommand[0][0]) { // empty call
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
    commandNotOk(activeCommand[0]);
    return;
  } 
  // Special handling for CLEAR because it clears 
  if(true == command.equals("CLEAR")) {      
    commandOk();
    return;
  }  
  connectAlive();
}

void commandNotOk(char *msg) {
    Serial.print("NOK_");          
    Serial.println(msg);
    Serial3.print("NOK_");
    Serial3.println(msg);              
    clearCommand();
}

void commandOk() {
    Serial.print("OK\n");          
    Serial3.print("OK\n");              
    clearCommand();
}

void processTimer() {
  unsigned long ts = millis();
  if(ts > timer.motorF_L) {
    actor.motorF_L = 0;
  }
  if(ts > timer.motorB_L) {
    actor.motorB_L = 0;
  }  
  if(ts > timer.motorF_R) {
    actor.motorF_R = 0;
  }  
  if(ts > timer.motorB_R) {
    actor.motorB_R = 0;
  }  
}

void processCommand() {  
  if(0 == activeCommand[0][0]) {
    return;
  }
  String command = String(activeCommand[0]);  
  if(command.equals("ID")) {
    Serial.print("Hallo\n");
    Serial3.print("Hallo\n");
    commandOk();    
  } else if(command.equals("FORWARD")) {
    forwardCommand();
    return;
  } else if(command.equals("RCIRCLE")) {
    rcircleCommand();
    return;
  } else if(command.equals("LCIRCLE")) {
    lcircleCommand();
    return;
  } else if(command.equals("LEFT")) {
    leftCommand();
    return;
  } else if(command.equals("RIGHT")) {
    rightCommand();
    return;
  } else if(command.equals("BACK")) {
    backCommand();
    return;
  } 
  clearCommand();    
}

boolean isConnectAlive() {

  unsigned long ts = millis();
  if(ts < timer.connectalive) {
    return true;
  }
  Serial.end();  
  Serial3.end();    
  setupActor();
  clearReadbuffer0();
  clearReadbuffer3();
  clearCommand();
  Serial.begin(115200);    
  Serial3.begin(115200);    
  return false;

}

void connectAlive() {
  unsigned long ts = millis();
  timer.connectalive = ts + CONNECTALIVE;
}

void processActor() {
  motorFRONT_LEFT.runSpeed(actor.motorF_L);
  motorBACK_LEFT.runSpeed(actor.motorB_L);
  motorFRONT_RIGHT.runSpeed(actor.motorF_R);
  motorBACK_RIGHT.runSpeed(actor.motorB_R);
}

void forwardCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = -speed;
  actor.motorB_L = -speed;
  actor.motorF_R = speed;
  actor.motorB_R = speed;
  setMotorTimer(timer + millis());
  commandOk();
}

void rcircleCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = -speed;
  actor.motorB_L = -speed;
  actor.motorF_R = -speed;
  actor.motorB_R = -speed;
  setMotorTimer(timer + millis());
  commandOk();  
}

void lcircleCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = speed;
  actor.motorB_L = speed;
  actor.motorF_R = speed;
  actor.motorB_R = speed;
  setMotorTimer(timer + millis());
  commandOk();  
}

void leftCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = speed;
  actor.motorB_L = -speed;
  actor.motorF_R = speed;
  actor.motorB_R = -speed;
  setMotorTimer(timer + millis());
  commandOk();  
}

void rightCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = -speed;
  actor.motorB_L = speed;
  actor.motorF_R = -speed;
  actor.motorB_R = speed;
  setMotorTimer(timer + millis());
  commandOk();  
}

void backCommand() {
  int speed = 100;
  unsigned long timer = 1000;
  if(0 != activeCommand[1]) {
    timer = atoi(activeCommand[1]);
  } 
  if(0 != activeCommand[2]) {
    speed = (unsigned long)atoi(activeCommand[2]);
  }
  actor.motorF_L = speed;
  actor.motorB_L = speed;
  actor.motorF_R = -speed;
  actor.motorB_R = -speed;
  setMotorTimer(timer + millis());
  commandOk();  
}

void setMotorTimer(unsigned long mtime) {
  timer.motorF_L = mtime;
  timer.motorB_L = mtime;
  timer.motorF_R = mtime;
  timer.motorB_R = mtime;
}
