/*
  7dtd-auto-miner
  0.0
*/

#include "Keyboard.h"
#include "Mouse.h"

#define pinBtnLeft 2
#define pinBtnRight 3
#define pinBtnTop 4
#define pinMine 5
#define pinWalk 6
#define pinSway A0
#define pinSpeed A1

const int swayDistancePlus = 50;
const int swayDistanceMinus = -50;

const int intervalInput = 500; // ms, interval of the input check loop
const int intervalSwayMin = 1;
const int intervalSwayMax = 10;
int intervalWalk = 10; // ms, interval of the walking loop

unsigned long currMs = 0;
unsigned long lastMain = 0;
unsigned long lastSway = 0;
unsigned long lastWalk = 0;

bool mining = false;
bool walking = false;
bool stateBtnLeft = false;
bool stateBtnRight = false;
bool stateBtnTop = false;
bool stateBtnMine = false;
bool stateBtnWalk = false;
int mouseSway = 0;
int walkSpeed = 0;

int swayPosition = 0;
int intervalSway = 10; // ms, will get modified during runtime
bool swayDirection = false; // false =  up, true = down

void setup() {
  // init Inputs
  pinMode(pinBtnLeft, INPUT_PULLUP);
  pinMode(pinBtnRight, INPUT_PULLUP);
  pinMode(pinBtnTop, INPUT_PULLUP);
  pinMode(pinMine, INPUT_PULLUP);
  pinMode(pinWalk, INPUT_PULLUP);

  Serial.begin(9600);

  // inint USB Hid
  Mouse.begin();
  Keyboard.begin();
}

void loop() {
  currMs = millis();
  if (currMs - lastMain >= intervalInput)
  {
    lastMain = currMs;

    updateInputStates();
    updateAnalogValues();

    //    Serial.println("### currMs - last ###");
    //    Serial.print(currMs - last);
    //    Serial.println("ms");

    //updateKeys();
  }
  updateKeys();
}

void updateInputStates()
{
  stateBtnLeft = !digitalRead(pinBtnLeft);
  stateBtnRight = !digitalRead(pinBtnRight);
  stateBtnTop = !digitalRead(pinBtnTop);
  stateBtnMine = !digitalRead(pinMine);
  stateBtnWalk = !digitalRead(pinWalk);

  //  Serial.println("### btnstates ###");
  //  Serial.println(stateBtnLeft);
  //  Serial.println(stateBtnRight);
  //  Serial.println(stateBtnTop);
  //  Serial.println(stateBtnMine);
  //  Serial.println(stateBtnWalk);
}

void updateAnalogValues()
{
  if (mining)
  {
    mouseSway = analogRead(pinSway);
    intervalSway = map(mouseSway, 0, 1023, intervalSwayMax, intervalSwayMin);
  }
  if (walking)
  {
    walkSpeed = analogRead(pinSpeed);
  }
}

void updateKeys()
{
  if (mining != stateBtnMine)
  {
    if (mining)
    {
      // was active deactivate
      mining = false;
      if (Mouse.isPressed(MOUSE_LEFT)) {
        Mouse.release(MOUSE_LEFT);
      }
    }
    else
    {
      // was active activate
      mining = true;
      if (!Mouse.isPressed(MOUSE_LEFT)) {
        Mouse.press(MOUSE_LEFT);
      }
    }
    Serial.print("## mining: ");
    Serial.println(mining ? "true" : "false");
  }

  if (mining)
  {
    processSway();

//    Serial.print("mining...");
//    Serial.println(mouseSway);
  }


  if (walking != stateBtnWalk)
  {
    if (walking)
    {
      // was active deactivate
      walking = false;
    }
    else
    {
      // was active activate
      walking = true;
    }

    Serial.print("## walking: ");
    Serial.println(walking ? "true" : "false");
  }

  if (walking)
  {
    Serial.print("walking...");
    Serial.println(walkSpeed);
  }
}

void processSway()
{
  if (currMs - lastSway >= intervalSway)
  {
    lastSway = currMs;
    if (swayPosition >= swayDistanceMinus && swayDirection == false)
    {
      if (swayPosition < swayDistancePlus)
      {
        swayPosition++;        
        Mouse.move(0, 1, 0);
      }
      else
      {
        swayPosition--;
        swayDirection = true;  
        Mouse.move(0, -1, 0);
      }
    }
    else if (swayPosition <= swayDistancePlus && swayDirection == true)
    {
      if (swayPosition > swayDistanceMinus)
      {
        swayPosition--;
        Mouse.move(0, -1, 0);
      }
      else
      {
        swayPosition++;
        swayDirection = false;   
        Mouse.move(0, 1, 0);
      }
    }
    
//    Serial.print("swayPos: ");
//    Serial.print(swayPosition);
//    Serial.print(" ");
//    for(int i = swayDistanceMinus; i < swayPosition; i++)
//    {
//      Serial.print("#");
//    }
//    Serial.println();
    
  }
}

