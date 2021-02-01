#include "HX711.h"

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

const int resinRelay = 11;
const int resinButton = 3;
const int hardRelay = 2;
const int hardButton = 4;
const int resetButton = 5;
const int resinLED = 8;
const int hardLED = 10;
//const int readyLED = 9;
const int cleanButton = 12;

boolean resinDispensed = false;

//comment out all the follwing lines except for the product you're using to set the mixing ratio
//double mixRatio = 0.45; //TotalBoat High Performance 2:1 Slow or Medium
//double mixRatio = 0.46; //TotalBoat High Performance 2:1 Fast
//double mixRatio = 0.18; //TotalBoat Traditional 5:1 Slow or Fast
//double mixRatio = 0.27; //TotalBoat Traditional Crystal Clear
//double mixRatio = 0.28; //TotalBoat Traditional Tropical Extra Slow
//double mixRatio = 0.83; //TotalBoat TableTop
//double mixRatio = 0.84; //TotalBoat MakerPoxy
double mixRatio = 0.28; //TotalBoat ThickSet
//double mixRatio = 0.44; //TotalBoat Penetrating Epoxy

float startMass;
float currentMass;
float targetMass;
float dispensedMass;

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  Serial.begin (9600);

  pinMode (resinRelay, OUTPUT);
  pinMode (hardRelay, OUTPUT);
  pinMode (resinLED, OUTPUT);
  pinMode (hardLED, OUTPUT);

  pinMode (resinButton, INPUT);
  pinMode (hardButton, INPUT);
  pinMode (resetButton, INPUT);
  pinMode (cleanButton, INPUT);

}

void loop() {
  digitalWrite (resinLED, HIGH); //default state should start by dispensing resin

  startMass = abs(scale.read());
  currentMass = startMass;

  while (digitalRead(resinButton)) { //user wants to get resin
    digitalWrite(resinRelay, HIGH);
    currentMass = abs(scale.read());
    dispensedMass = startMass - currentMass;
    
    //Serial.println ("dispensing resin");
    //Serial.println (dispensedMass);
    resinDispensed = true;
  }
  digitalWrite(resinRelay, LOW);
  digitalWrite (resinLED, LOW);
  digitalWrite (hardLED, HIGH);
  targetMass = (float) (dispensedMass * mixRatio); //new target mass

  //debug info, turn off for normal operation.
  /*
  Serial.println("resin dispensed");
  Serial.println (dispensedMass);
  Serial.println ("target mass");
  Serial.println(targetMass);
  Serial.println("=============");
*/

  while (!digitalRead(hardButton) && resinDispensed) {
    //wait for the hardener button to be pressed
    delay(5);
    currentMass = abs(scale.read());
    startMass = currentMass;
    Serial.println ("waiting for hardener");

    if (digitalRead(resetButton)){ //if user wants to reset, this should take us back to the beginning.
      resinDispensed = false;
      Serial.println ("reset");
      break;
    }
  }

  if (digitalRead(hardButton) && resinDispensed) {
    digitalWrite (hardRelay, HIGH);
    Serial.println (targetMass);
    while ((startMass - currentMass) < targetMass) {
      currentMass = abs(scale.read());
      Serial.println(startMass - currentMass);
      resinDispensed = false;
      //Serial.println ("dispensing hardener");
    }
    digitalWrite (hardRelay, LOW);
    //break;
  }

  digitalWrite (hardLED, LOW);
  Serial.println ("Ready");

}
