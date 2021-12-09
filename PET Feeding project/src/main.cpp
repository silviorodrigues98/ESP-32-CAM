/*
   Original sourse: https://github.com/RoboticsBrno/ESP32-Arduino-Servo-Library

   This is Arduino code to control Servo motor with ESP32 boards.
   Watch video instruction for this code: https://youtu.be/cDbd0ASkMog

   Written/updated by Ahmad Shamshiri for Robojax Video channel www.Robojax.com
   Date: Dec 17, 2019, in Ajax, Ontario, Canada
   Permission granted to share this code given that this
   note is kept with the code.
   Disclaimer: this code is "AS IS" and for educational purpose only.
   this code has been downloaded from http://robojax.com/learn/arduino/

   Get this code and other Arduino codes from Robojax.com
  Learn Arduino step by step in structured course with all material, wiring diagram and library
  all in once place. Purchase My course on Udemy.com http://robojax.com/L/?id=62

****************************
  Get early access to my videos via Patreon and have  your name mentioned at end of very
  videos I publish on YouTube here: http://robojax.com/L/?id=63 (watch until end of this video to list of my Patrons)
****************************

  or make donation using PayPal http://robojax.com/L/?id=64

 *  * This code is "AS IS" without warranty or liability. Free to be used as long as you keep this note intact.*
   This code has been download from Robojax.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
  *  * Some new features have been added to the code, like wifi's connection, Adafuit and IFTT integrations(for google assistant).
    These were added by SILVIO CORREA.
*/
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo_ESP32.h>

/************************* Configuração Servo *********************************/

static const int servoPin = 14; // printed G14 on the board

Servo_ESP32 servo1;

int angle = 0;     // Start Angle
int angleStep = 5; // How much it moves per loop

int angleMin = 0;   // Position that it will come back to
int angleMax = 180; // Position where it is going to go

int delayTime = 20; // Simulates servo's speed

/************************* Configuração Wifi *********************************/

#define WLAN_SSID "*******"      // Nome da Rede Wifi
#define WLAN_PASS "***********" // Senha da Rede Wifi

/************************* Adafruit.io Configuração *********************************/

#define AIO_SERVER "io.adafruit.com" // Adafruit Servidor
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "***********"        // Insira o usuario criado na adafruit io
#define AIO_KEY "******************************************** // Insira a chave de comunicação obtida na adafruit io

/************************* Configuração do status *********************************/

int conectado = 1;
int conexao = 0;
int servoStatus;
#define LED_conexao 33
unsigned long anterior = 0;
unsigned long intervalo = 15000;
unsigned long atual;

// Definição do Wifi Client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Váriáveis criadas para comunlicação com o MQTT

Adafruit_MQTT_Subscribe servoPet1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/servopet");

// Funções
void MQTT_connect(); // Determina a conexão MQTT
void Conectar();     // Verifica a conexão o Wifi

void setup()
{
  Serial.begin(115200);
  servo1.attach(servoPin);
  pinMode(LED_conexao, OUTPUT);
  // Desliga o LED de status de conexão do Wifi
  digitalWrite(LED_conexao, HIGH);
  // Declaração de Tópicos de Leitura
  mqtt.subscribe(&servoPet1);
}

void loop()
{
  Conectar();
  MQTT_connect();

  // Leitura do valor recebido através da Adafruit IO
  Adafruit_MQTT_Subscribe *subscription;
  if ((subscription = mqtt.readSubscription(100)))
  {
    if (subscription == &servoPet1)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)servoPet1.lastread);
      servoStatus = atoi((char*)servoPet1.lastread);
    }
    if (servoStatus == 1)
    {
      for (int angle = 0; angle <= angleMax; angle += angleStep)
      { // Make servo go to position
        servo1.write(angle);
        Serial.println(angle);
        delay(delayTime);
      }
      for (int angle = 180; angle >= angleMin; angle -= angleStep)
      { // Make servo turn back to it original position
        servo1.write(angle);
        Serial.println(angle);
        delay(delayTime);
      }
    }
  }
}


void MQTT_connect() {
  int8_t ret;
  //Conectando MQTT
  if (mqtt.connected()) {
    if (conectado) {
      conectado = 0;
      Serial.println("MQTT Conectado!");
    }
    return;
  }
  Serial.print("Conectando MQTT...");
  uint8_t retries = 3;
  if ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reconexao ao MQTT em 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    conectado = 1;
    //retries--;
    if (retries == 0) {
      //retries = 3;
    }
  }
}

void Conectar() {
  //Verifica a conexão wifi
  if (WiFi.status() != WL_CONNECTED) {
    conexao = 0;
    //Conectando ao Wifi
    Serial.println();
    Serial.println();
    Serial.print("Conectando a rede: ");
    Serial.println(WLAN_SSID);
    //Inicializa Conexão Wifi
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_conexao, LOW);
      delay(100);
      digitalWrite(LED_conexao, HIGH);
      delay(100);
      Serial.print(".");
      conexao++;
      if (conexao == 30) {
        ESP.restart();
      }
    }
    Serial.println("WiFi conectado!");
    Serial.println("Endereco de IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_conexao, LOW);
    conectado = 1;
    Serial.println();
  }
}