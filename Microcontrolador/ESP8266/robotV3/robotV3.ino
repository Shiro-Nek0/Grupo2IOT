#include "SSD1306Wire.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

/* WS2812B */
#define NUM_LEDS 1
#define DATA_PIN D0
CRGB leds[NUM_LEDS];

/* OLED */
SSD1306Wire display(0x3c, SDA, SCL);

/* SERVO */
#define servoPin D4
Servo tiltservo;

/* DHT */
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* MOTORES */
#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8
int outputPins[] = {IN1, IN2, IN3, IN4};

/* Wifi */
const char *ssid = "*****";
const char *password = "*****";
WiFiClient espClient;

/* MQTT */
#define MSG_BUFFER_SIZE 50
PubSubClient MQTTclient(espClient);
const char *mqtt_server = "*****";
char msg[MSG_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 5; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
  }
  display.init();
  display.flipScreenVertically();
  dht.begin();
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  tiltservo.attach(servoPin);
  tiltservo.write(90);

  setup_wifi();
  MQTTclient.setServer(mqtt_server, 1883);
  MQTTclient.setCallback(onMessage);
}

unsigned long lastMsg = 0;
void loop() {
  if (!MQTTclient.connected()) {
    reconnect();
  }
  MQTTclient.loop();

  unsigned long now = millis(); // asynchronous timer
  if (now - lastMsg > 2000) {
    lastMsg = now;
    sendData();
  }
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  display.clear();
  display.drawString(0, 0, "Conectando a WIFI");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("Connected to WIFI, IP:");
  Serial.println(WiFi.localIP());

  display.clear();
  display.drawString(0, 0, "Conectado en");
  display.drawString(0, 10, WiFi.localIP().toString());
  display.display();
}

void onMessage(char *topic, byte *payload, unsigned int length) {
  Serial.print(topic);
  Serial.print("->");

  String messageTemp = "";
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  Serial.println(messageTemp);
  if (String(topic) == "grupo2/control") {
    int action = messageTemp.toInt();
    motorHandler(action);
<<<<<<< HEAD
  } else if (String(topic) == "grupo2/servo") {
    int value = messageTemp.toInt();
    if (value > 180) {
      value = 180;
    } else if (value < 0) {
      value = 0;
      == == == =
    } else if (String(topic) == "grupo2/servo") {
      int value = messageTemp.toInt();
      if (value > 180) {
        value = 180;
      } else if (value < 0) {
        value = 0;
>>>>>>> c6101d04e108caab2a886a9f6e92adecff4534ca
      }
      tiltservo.write(value);
    }
  }

  void reconnect() {
    while (!MQTTclient.connected()) {
      Serial.print("Conectando a MQTT...");

      display.clear();
      display.drawString(0, 0, "Conectando a MQTT");

      String clientId = "RobotClient-0";
      if (MQTTclient.connect(clientId.c_str())) {
        Serial.println("Successful MQTT connection");
        MQTTclient.subscribe("grupo2/control");
        MQTTclient.subscribe("grupo2/servo");

        display.clear();
        display.drawString(0, 0, "Conectado a MQTT");
        display.display();
      } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.print(MQTTclient.state());
        Serial.println(" trying again in 2 seconds");

        display.clear();
        display.drawString(0, 0, "Error conexion MQTT");
        display.display();
        delay(2000);
      }
    }
  }

  void motorHandler(int dir) {
    switch (dir) {
    case 0:
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      Serial.println("Stop");
      break;
    case 1:
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
<<<<<<< HEAD
      Serial.println("Backward");
      == == == =
                   Serial.println("Backward"); 
>>>>>>> c6101d04e108caab2a886a9f6e92adecff4534ca
      break;
    case 2:
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      Serial.println("Forward");
      break;
    case 3:
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      Serial.println("Right");
      break;
    case 4:
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      Serial.println("Left");
      break;
    }
  }

  void sendData() {
    float t = dht.readTemperature();
    MQTTclient.publish("grupo2/dhttemp", String(t).c_str());

    float h = dht.readHumidity();
    MQTTclient.publish("grupo2/dhthum", String(h).c_str());

    int b = analogRead(A0);
    MQTTclient.publish("grupo2/robbatt", String(b).c_str());
  }