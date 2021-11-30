//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
char receivedValue;

#define LED_BUILTIN 33

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-Cam SILVIO"); //Bluetooth device name
  Serial.println("Dispositivo iniciado!");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

receivedValue = (char)SerialBT.read();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    if (receivedValue == '1') {
      digitalWrite(LED_BUILTIN, HIGH);
      SerialBT.println("LED LIGADO");
      
    }
    if (receivedValue == '2') {
      digitalWrite(LED_BUILTIN, LOW);
      SerialBT.println("LED DESLIGADO");
    }
  }
}
