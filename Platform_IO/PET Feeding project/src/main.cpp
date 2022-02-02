#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo_ESP32.h>
#include "time.h"

/************************* Configuração Servo *********************************/
Servo_ESP32 servo1;
static const byte servoPin = 14; // printed G14 on the board
byte angle = 0;     // Start Angle
byte angleStep = 5; // How much it moves per loop
byte angleMin = 0;   // Position that it will come back to
byte angleMax = 180; // Position where it is going to go
byte delayTime = 20; // Simulates servo's speed

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
byte binaryStatus;
byte morningStatus;
byte afternoonStatus;
byte eveningStatus;
#define LED_conexao 33
unsigned long millisCounter = millis();
unsigned long statusUpdateDelay = 10000;

/************************* All Adafruit Feeds, to get and send data *********************************/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe binarySwitches = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet_binario");
Adafruit_MQTT_Subscribe morningFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet_manha");
Adafruit_MQTT_Subscribe afternoonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet_tarde");
Adafruit_MQTT_Subscribe eveningFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet_noite");
Adafruit_MQTT_Subscribe servoSpeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feedpet_velocidade");
Adafruit_MQTT_Publish petFeederStatus = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/feedpet_isOnline");
Adafruit_MQTT_Publish onlineSerial = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/feedpet_onlineSerial");

/*************************  Atualização de hora e data em tempo real *********************************/
const long  gmtOffset_sec = -10800;//Ajuste em relação ao UTC(GMT-3);
byte daylightOffset_sec = 0;//Horário de verão
const char* ntpServer = "pool.ntp.org";
byte currentHour;

/************************* Bools definitions *********************************/
bool isFirst = true;
bool autoClicked = true;

/************************* Functions declarations *********************************/
void autoFeed();
void feedNow();
void printLocalTime();
void Conectar();
void MQTT_connect();
void statusUpdate();
void onlineSerialChar();
void onlineSerialInt();

/************************* PROGRAM  BEGIN *********************************/
void setup()
{
  Serial.begin(115200);
  servo1.attach(servoPin);
  servo1.write(0);
  pinMode(LED_conexao, OUTPUT);
  digitalWrite(LED_conexao, HIGH);
  //
  mqtt.subscribe(&binarySwitches);
  mqtt.subscribe(&morningFeed);
  mqtt.subscribe(&afternoonFeed);
  mqtt.subscribe(&eveningFeed);
  mqtt.subscribe(&servoSpeed);

  //
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //
  Conectar();
  MQTT_connect();
  statusUpdate();
}

void loop()
{
  /************************* Connect Wifi, MQTT and GMT servers *********************************/
  servo1.write(0);
  Conectar();
  MQTT_connect();
  statusUpdate();

  /************************* Get and saves real time data from Adafruit *********************************/
  Adafruit_MQTT_Subscribe *subscription;
  if ((subscription = mqtt.readSubscription(100)))
  {
    Serial.print(F("Got: "));
    Serial.println((char *)binarySwitches.lastread);
    binaryStatus = atoi((char *)binarySwitches.lastread);
    Serial.println((char *)morningFeed.lastread);
    morningStatus = atoi((char *)morningFeed.lastread);
    Serial.println((char *)afternoonFeed.lastread);
    afternoonStatus = atoi((char *)afternoonFeed.lastread);
    Serial.println((char *)eveningFeed.lastread);
    eveningStatus = atoi((char *)eveningFeed.lastread);
    Serial.println((char *)servoSpeed.lastread);
    delayTime = atoi((char *)servoSpeed.lastread);
  }

  /************************* Testing and interacting with Conditions From Adafruit *********************************/
  switch (binaryStatus) {//Tests all binary switches
    case 1://Daylight Save Mode On
      daylightOffset_sec = 3600;
      break;
    case 0:
      daylightOffset_sec = 0;
      break;
    case 2://AutoFeedMode On
      autoFeed();
      break;
    case 3:
      autoClicked = true;
      break;
    case 4://FeedNow On
      feedNow();
      break;
  }
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
      delay(100);
      digitalWrite(LED_conexao, HIGH);
      delay(100);
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
}

void autoFeed() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  byte currentHour =  atoi((char *)timeHour);
  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  byte currentMinute =  atoi((char *)timeMinute);
  char timeSeconds[3];
  strftime(timeSeconds, 3, "%S", &timeinfo);
  byte currentSecond =  atoi((char *)timeSeconds);
  if (currentHour == morningStatus || currentHour == afternoonStatus || currentHour == eveningStatus) {
    if (currentMinute == 0 && currentSecond == 0) {
      feedNow();
    }
  }
  char* nextFeeds;
  sprintf(nextFeeds, "SUCESSO! SEU PET SERA ALIMENTADO ÀS %u, %u e %u HORAS.", morningStatus, afternoonStatus, eveningStatus);
  if (autoClicked) {
    onlineSerialChar(nextFeeds);
    autoClicked = false;
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

void onlineSerialInt(int message) {
  int sendThisMessage = message;
  Serial.print(sendThisMessage);
  if (! onlineSerial.publish(sendThisMessage)) {
    Serial.println("Failed");
  } else {
    Serial.println("OK!");
  }
}