#include "BluetoothSerial.h"
#include <string.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//SETUP

#define LED_BUILTIN  33
#define LED_FLASH 4
#define GPIO 2
char* ledB; 
char* ledF;
char* gpio;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
  pinMode(GPIO, OUTPUT);
}

void loop() {
   ledB  = (char*)SerialBT.read();
   ledF  = (char*)SerialBT.read();
   gpio  = (char*)SerialBT.read();
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    if(ledB  == "Ligar LED") {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if(ledB  == "Desligar LED") {
      digitalWrite(LED_BUILTIN, LOW);
    }
    if(ledF  == "Ligar FLASH") {
      digitalWrite(LED_FLASH, HIGH);
    }
    if(ledF  == "Desligar FLASH") {
      digitalWrite(LED_FLASH, LOW);
    }
    if(gpio  == "Ligar GPIO") {
      digitalWrite(GPIO, HIGH);
    }
    if(gpio  == "Desligar GPIO") {
      digitalWrite(GPIO, LOW);
    }
  }
  delay(20);
}
