// Bibliotecas utilizadas
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Definição dos pinos
#define LED_conexao 33
#define LED_azul 2
#define LED_vermelho 12

char *LED_azulSTATUS;
char *LED_vermelhoSTATUS;

#define WLAN_SSID "akso"      // Nome da Rede Wifi
#define WLAN_PASS "111333555" // Senha da Rede Wifi

/************************* Adafruit.io Configuração *********************************/

#define AIO_SERVER "io.adafruit.com" // Adafruit Servidor
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "**********"        // Insira o usuario criado na adafruit io
#define AIO_KEY "**********************" // Insira a chave de comunicação obtida na adafruit io

// Váriáveis utilizadas

int conectado = 1;
int conexao = 0;

unsigned long anterior = 0;
unsigned long intervalo = 15000;
unsigned long atual;

// Definição do Wifi Client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Váriáveis criadas para comunlicação com o MQTT
Adafruit_MQTT_Publish sensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Status Alarme"); // Crie aqui sua variavel
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_azul");
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led_vermelho"); // a palavra feeds deve estar em todos

// Funções
void MQTT_connect(); // Determina a conexão MQTT
void Conectar();     // Verifica a conexão o Wifi

void setup()
{

  Serial.begin(115200);

  // Definição de saida e entrada de cada pino
  pinMode(LED_conexao, OUTPUT);
  pinMode(LED_azul, OUTPUT);
  pinMode(LED_vermelho, OUTPUT);

  // Desliga o LED de status de conexão do Wifi
  digitalWrite(LED_conexao, HIGH);

  // Declaração de Tópicos de Leitura
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);
}

void loop()
{

  Conectar();
  MQTT_connect();

  // Leitura do valor recebido através da Adafruit IO
  Adafruit_MQTT_Subscribe *subscription;
  if ((subscription = mqtt.readSubscription(100)))
  {
    if (subscription == &Light1)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(LED_azul, Light1_State);
      LED_azulSTATUS = (char *)Light1.lastread;
      Serial.print("STATUS LED: ");
      Serial.println(LED_azulSTATUS);
    }
    if (subscription == &Light2)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(LED_vermelho, Light2_State);
      LED_vermelhoSTATUS = (char *)Light2.lastread;
      Serial.print("STATUS LED: ");
      Serial.println(LED_vermelhoSTATUS);
    }
  }
  if (LED_azulSTATUS == "11")
  {
    digitalWrite(LED_azul, HIGH);
    Serial.println("LIGADO");
  }
  if (LED_azulSTATUS == "00")
  {
    digitalWrite(LED_azul, LOW);
    Serial.println("DESLIGADO");
  }
  if (LED_vermelhoSTATUS == "11")
  {
    digitalWrite(LED_vermelho, HIGH);
    Serial.println("LIGADO");
  }
  if (LED_vermelhoSTATUS == "00")
  {
    digitalWrite(LED_vermelho, LOW);
    Serial.println("DESLIGADO");
  }
}
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