#include <Arduino.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo_ESP32.h>
#include "time.h"

/************************* Functions declarations *********************************/
void autoFeed();
void feedNow();
void printLocalTime();
void Conectar();
void MQTT_connect();
void statusUpdate();
void onlineSerialChar();
void workOnAdafruitData();
void subscribeFeeds();

/************************* Configuração Servo *********************************/
Servo_ESP32 servo1;
static const int servoPin = 14; // printed G14 on the board
int angle = 0;     // Start Angle
int angleStep = 5; // How much it moves per loop
int angleMin = 0;   // Position that it will come back to
int angleMax = 180; // Position where it is going to go
int delayTime = 100; // Simulates servo's speed

/************************* Adafruit.io Configuração *********************************/
#define AIO_SERVER "io.adafruit.com" // Adafruit Servidor
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "AncapCap" // Insira o usuario criado na adafruit io
#define AIO_KEY "aio_Mvrd117CFOtYiHOhYHjT9zrnC5cB"   // Insira a chave de comunicação obtida na adafruit io

/************************* Configuração Wifi *********************************/
#define WLAN_SSID "akso"     // Nome da Rede Wifi
#define WLAN_PASS "111333555" // Senha da Rede Wifi
WiFiClient client;

/************************* Configuração do status Wifi *********************************/
byte conectado = 1;
byte conexao = 0;
#define LED_conexao 33
#define BUZZER_conexao 2
#define EXTERNALLED_conexao 15

/************************* User Control Variables *********************************/
byte morningStatus = 8;
byte afternoonStatus = 13;
byte eveningStatus = 18;
byte feedimmediately;
byte modeAuto;
byte daySaveMode;

/************************* Counter and Delay Variables *********************************/
unsigned long millisCounter = millis();
unsigned long statusUpdateDelay = 10000;

/************************* All Adafruit Feeds, to get and send data *********************************/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe morningFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-manha");
Adafruit_MQTT_Subscribe afternoonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-tarde");
Adafruit_MQTT_Subscribe eveningFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-noite");
Adafruit_MQTT_Subscribe servoSpeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-velocidade");
Adafruit_MQTT_Publish petFeederStatus = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/feedpet-isonline");
Adafruit_MQTT_Subscribe autoMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-modoauto");
Adafruit_MQTT_Subscribe instantFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-alimentar");
Adafruit_MQTT_Subscribe daylightSavingMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet-horarioverao");
Adafruit_MQTT_Publish onlineSerial = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/feedpet-onlineserial");

/*************************  Atualização de hora e data em tempo real *********************************/
const long  gmtOffset_sec = -10800;//Ajuste em relação ao UTC(GMT-3);
byte daylightOffset_sec = 0;//Horário de verão
const char* ntpServer = "pool.ntp.org";
byte currentHour;

/************************* Bools definitions *********************************/
bool isFirst = true;

/************************* PROGRAM  BEGIN *********************************/
void setup()
{
  Serial.begin(115200);
  servo1.attach(servoPin);
  servo1.write(0);
  //
  pinMode(LED_conexao, OUTPUT);
  pinMode(BUZZER_conexao, OUTPUT);
  pinMode(EXTERNALLED_conexao, OUTPUT);
  digitalWrite(LED_conexao, HIGH);
  //
  subscribeFeeds();
  //
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //
  Conectar();
  MQTT_connect();
  statusUpdate();
}

void loop()
{
  servo1.write(0);
  /************************* Connect Wifi, MQTT and GMT servers *********************************/
  Conectar();
  MQTT_connect();
  statusUpdate();

  /************************* Get and saves real time data from Adafruit *********************************/
  workOnAdafruitData();

}

/************************* FUNCTIONS *********************************/
void MQTT_connect()
{
  int8_t ret;
  // Conectando MQTT
  if (mqtt.connected())
  {
    if (conectado)
    {
      conectado = 0;
      Serial.println("MQTT Conectado!");
    }
    return;
  }
  Serial.print("Conectando MQTT...");
  uint8_t retries = 3;
  if ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reconexao ao MQTT em 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    conectado = 1;
    // retries--;
    if (retries == 0)
    {
      // retries = 3;
    }
  }
}

void Conectar()
{
  // Verifica a conexão wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    conexao = 0;
    // Conectando ao Wifi
    Serial.println();
    Serial.println();
    Serial.print("Conectando a rede: ");
    Serial.println(WLAN_SSID);
    // Inicializa Conexão Wifi
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(LED_conexao, LOW);
      digitalWrite(BUZZER_conexao, LOW);
      delay(300);
      digitalWrite(LED_conexao, HIGH);
      digitalWrite(BUZZER_conexao, HIGH);
      delay(300);
      Serial.print(".");
      conexao++;
      if (conexao == 30)
      {
        ESP.restart();
      }
    }
    Serial.println("WiFi conectado!");
    Serial.println("Endereco de IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_conexao, LOW);
    digitalWrite(BUZZER_conexao, HIGH);
    digitalWrite(EXTERNALLED_conexao, HIGH);
    conectado = 1;
    Serial.println();
  }
}

void feedNow() {
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
  onlineSerialChar("SEU PET FOI ALIMENTADO COM SUCESSO!!!");
  digitalWrite(BUZZER_conexao, LOW);
  delay(20);
  digitalWrite(BUZZER_conexao, HIGH);
}

void autoFeed() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  int currentHour =  atoi((char *)timeHour);
  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  int currentMinute =  atoi((char *)timeMinute);
  char timeSeconds[3];
  strftime(timeSeconds, 3, "%S", &timeinfo);
  int currentSecond =  atoi((char *)timeSeconds);
  if (currentHour == morningStatus || currentHour == afternoonStatus || currentHour == eveningStatus) {
    if (currentMinute == 0 && currentSecond == 0) {
      feedNow();
    }
  }
}

void statusUpdate() {
  if (millis() - millisCounter >= statusUpdateDelay) {
    millisCounter = millis();
    if (isFirst) {
      byte isOn = 1;
      if (! petFeederStatus.publish(isOn)) {
        Serial.println("Failed");
      } else {
        Serial.println("OK!");
      }
      isFirst = false;
    } else {
      byte isOff = 0;
      if (! petFeederStatus.publish(isOff)) {
        Serial.println("Failed");
      } else {
        Serial.println("OK!");
      }
      isFirst = true;
    }
  }
}

void onlineSerialChar(char* message) {
  char* sendThisMessage = message;
  Serial.print(sendThisMessage);
  if (! onlineSerial.publish(sendThisMessage)) {
    Serial.println("Failed");
  } else {
    Serial.println("OK!");
  }
}

void workOnAdafruitData() {
  Adafruit_MQTT_Subscribe *subscription;
  if ((subscription = mqtt.readSubscription(100)))
  {
    Serial.print(F("Got: "));
    if (subscription == &morningFeed) {
      Serial.println((char *)morningFeed.lastread);
      morningStatus = atoi((char *)morningFeed.lastread);
      String sendThisMessage = "HORÁRIO DA MANHÃ DEFINIDO PARA " + String(morningStatus) +"h";
      onlineSerialChar((char*)sendThisMessage.c_str());
    }
    if (subscription == &afternoonFeed) {
      Serial.println((char *)afternoonFeed.lastread);
      afternoonStatus = atoi((char *)afternoonFeed.lastread);
      String sendThisMessage = "HORÁRIO DA TARDE DEFINIDO PARA " + String(afternoonStatus) +"h";
      onlineSerialChar((char*)sendThisMessage.c_str());
    }
    if (subscription == &eveningFeed) {
      Serial.println((char *)eveningFeed.lastread);
      eveningStatus = atoi((char *)eveningFeed.lastread);
      String sendThisMessage = "HORÁRIO DA NOITE DEFINIDO PARA " + String(eveningStatus) +"h";
      onlineSerialChar((char*)sendThisMessage.c_str());
    }
    if (subscription == &servoSpeed) {
      Serial.println((char *)servoSpeed.lastread);
      delayTime = atoi((char *)servoSpeed.lastread);
      String sendThisMessage = "VELOCIDADE ALTERADA PARA: " + String(delayTime) +"ms";
      onlineSerialChar((char*)sendThisMessage.c_str());
    }
    if (subscription == &instantFeed) {
      Serial.println((char *)instantFeed.lastread);
      feedimmediately = atoi((char *)instantFeed.lastread);
      if (feedimmediately == 1) {
        feedNow();
      }
    }
    if (subscription == &autoMode) {
      Serial.println((char *)autoMode.lastread);
      modeAuto = atoi((char *)autoMode.lastread);
      if (modeAuto == 1) {
        String sendThisMessage = "PERFEITO! SEU PET SERÁ ALIMENTADO ÁS: " + String(morningStatus) + ", " + String(afternoonStatus) + " e " + String(eveningStatus) + " HORAS. ";
        onlineSerialChar((char*)sendThisMessage.c_str());
      } else if (modeAuto == 0) {
        onlineSerialChar("MODO AUTOMÁTICO DESATIVADO COM SUCESSO");
      }
    }
    if (subscription == &daylightSavingMode) {
      Serial.println((char *)daylightSavingMode.lastread);
      daySaveMode = atoi((char *)daylightSavingMode.lastread);
      if (daySaveMode == 1) {
        onlineSerialChar("MODO HORÁRIO DE VERÃO ATIVADO COM SUCESSO");
      } else if (daySaveMode == 0) {
        onlineSerialChar("HORÁRIO DE VERÃO FOI DESATIVADO");
      }
    }
  }
}

void subscribeFeeds() {
  mqtt.subscribe(&morningFeed);
  mqtt.subscribe(&afternoonFeed);
  mqtt.subscribe(&eveningFeed);
  mqtt.subscribe(&autoMode);
  mqtt.subscribe(&daylightSavingMode);
  mqtt.subscribe(&instantFeed);
  mqtt.subscribe(&servoSpeed);
}