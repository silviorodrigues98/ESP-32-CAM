#include <Arduino.h>

//SETUP

#define GPIO_1 15
int timeDelay = 2000; 

void setup() {
  // put your setup code here, to run once:
  pinMode(GPIO_1, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(GPIO_1, HIGH);
  delay(timeDelay);
  digitalWrite(GPIO_1, LOW);
  delay(timeDelay);
}