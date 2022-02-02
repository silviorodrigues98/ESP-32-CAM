#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//SETUP

#define LED_BUILTIN  33
char onOff;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
   onOff = (char)SerialBT.read();
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    if(onOff == 'b') {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if(onOff == 'a') {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  delay(20);
}
