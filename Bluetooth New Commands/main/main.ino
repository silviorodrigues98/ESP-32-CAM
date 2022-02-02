#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//SETUP

#define LED_BUILTIN  33
#define LED_FLASH 4

String command;
char* statusLED = "LED Apagado ";
char* statusFLASH = "FLASH Apagado ";

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-CAM Silvio"); //Bluetooth device name
  Serial.println("\nBluetooth ativado, pode parear!!!");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  command  = String(SerialBT.read());
  if (command == "TESTE") {
    Serial.println("DEU CERTO");
  }
  delay(20);
}
