#include <Arduino.h>
#define LED_BUILTIN 33
#define FLASH 4
int timeDelay = 500;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(timeDelay);
  digitalWrite(LED_BUILTIN, LOW);
  delay(timeDelay);
  digitalWrite(FLASH, HIGH);
  delay(timeDelay);
  digitalWrite(FLASH, LOW);
  delay(timeDelay);
}