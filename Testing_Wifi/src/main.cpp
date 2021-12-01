#include <WiFi.h>;

const char* ssid = "akso";
const char* password = "111333555";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.print(" ");
  Serial.println("Connected to WiFi network");
  Serial.print("IP adress: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  if ((WiFi.status() == WL_CONNECTED)) {
    Serial.println("You can try to ping me");
    delay(5000);
  }
  else {
    Serial.println("Connection lost");
  }

}