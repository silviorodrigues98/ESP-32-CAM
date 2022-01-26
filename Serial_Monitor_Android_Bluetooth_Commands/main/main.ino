#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT; 

//SETUP

#define LED_BUILTIN  33
#define LED_FLASH 4
#define GPIOOUT 2
char command;
char* statusLED = "LED Apagado ";
char* statusFLASH = "FLASH Apagado ";
char* statusGPIOOUT = "GPIO2OUT Desativado ";


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-CAM Silvio"); //Bluetooth device name
  Serial.println("\nBluetooth ativado, pode parear!!!");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
  pinMode(GPIOOUT, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  command  = (char)SerialBT.read();
  if (command == 'H') {
    SerialBT.println("COMANDOS:\nA/B = LED \nC/D = FLASH\nD/E = \nGPIO2OUT = E/F\nS = STATUS\nH = HELP");
  }
  if (command == 'S') {
    SerialBT.println(statusLED);
    SerialBT.println(statusFLASH);
    SerialBT.println(statusGPIOOUT);
  }
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    if (command  == 'A') {
      digitalWrite(LED_BUILTIN, LOW);
      statusLED = "LED Aceso";
      SerialBT.println(statusLED);
    }
    if (command  == 'B') {
      digitalWrite(LED_BUILTIN, HIGH);
      statusLED = "LED Apagado";
      SerialBT.println(statusLED);
    }
    if (command  == 'C') {
      digitalWrite(LED_FLASH, HIGH);
      statusFLASH = "FLASH Aceso";
      SerialBT.println(statusFLASH);
    }
    if (command  == 'D') {
      digitalWrite(LED_FLASH, LOW);
      statusFLASH = "FLASH Apagado";
      SerialBT.println(statusFLASH);
    }
    if (command  == 'E') {
      digitalWrite(GPIOOUT, HIGH);
      statusGPIOOUT = "GPIO2OUT Ativado";
      SerialBT.println(statusGPIOOUT);
    }
    if (command  == 'F') {
      digitalWrite(GPIOOUT, LOW);
      statusGPIOOUT = "GPIO2OUT Desativado";
      SerialBT.println(statusGPIOOUT);
    }
  }
  delay(20);
}
