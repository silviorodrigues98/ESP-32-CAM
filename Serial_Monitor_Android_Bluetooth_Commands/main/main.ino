#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
bool isConnected = false;
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Client Connected");
    isConnected = true;
  }
  else{
    isConnected = false;
  }
}


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
  if (!SerialBT.begin("ESP32")) {
    Serial.println("Ocorreu um erro ao se inicializar o Bluetooth");
  } else {
    Serial.println("Bluetooth inicializado, pode parear!!!");
  }
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
  pinMode(GPIOOUT, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  SerialBT.register_callback(callback);


}

void loop() {
  command  = (char)SerialBT.read();
  if (command == 'H' || isConnected) {
    SerialBT.println("COMANDOS:\nA/B = LED \nC/D = FLASH\nGPIO2OUT = E/F\nS = STATUS\nH = HELP");
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
