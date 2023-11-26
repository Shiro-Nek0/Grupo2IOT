#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <FastLED.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

/* WS2812B */
#define NUM_LEDS 1
#define DATA_PIN 3
CRGB leds[NUM_LEDS];

/* OLED */
#define SCREEN_ADDRESS 0x3C
SSD1306AsciiAvrI2c oled;

/* DHT */
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* MOTORES */
#define EN 5
#define EN2 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
int driverPins[] = {IN1, IN2, IN3, IN4, EN, EN2};

const int MAX_INPUT_LENGTH = 256;
char serialInput[MAX_INPUT_LENGTH];
int serialIndex = 0;

void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(driverPins[i], OUTPUT);
  }
  Serial.begin(9600);
  dht.begin();
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  oled.begin(&Adafruit128x64, SCREEN_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();
}

void loop() {
  while (Serial.available()) {
    char receivedChar = Serial.read();

    if (receivedChar == '\n') { // esperar a fin de linea
      serialInput[serialIndex] = '\0';
      processMessage(serialInput); // procesar mensaje
      serialIndex = 0;
    } else if (serialIndex < MAX_INPUT_LENGTH - 1) {
      serialInput[serialIndex] = receivedChar;
      serialIndex++;
    }
  }
}

void processMessage(const char *message) {
  char type[2];
  char action[20];
  char value[MAX_INPUT_LENGTH];
  //(HORRIBLE IMPLEMENTACION MODIFICAR ᗜˬᗜ)
  if (sscanf(message, "%1s:%[^=]=%[^\n]", type, action, value) == 3) {
    if (type[0] == 'M') {             // tipo de mensaje: motor
      if (strcmp(action, "R") == 0) { // tipo de movimiento: right
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        Serial.print("Right: ");
        Serial.println(value);
      } else if (strcmp(action, "L") == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        Serial.print("Left: ");
        Serial.println(value);
      } else if (strcmp(action, "FW") == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        Serial.print("Forward: ");
        Serial.println(value);
      } else if (strcmp(action, "BW") == 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        Serial.print("Backward: ");
        Serial.println(value);
      } else if (strcmp(action, "STOP") == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        Serial.print("Stop: ");
        Serial.println(value);
      }
      analogWrite(EN, atoi(value));
      analogWrite(EN2, atoi(value));
    } else if (type[0] == 'S') { // Servo message (arreglar)
      if (strcmp(action, "X") == 0) {
        Serial.print("Absolute X: ");
        Serial.println(value);
      } else if (strcmp(action, "Y") == 0) {
        Serial.print("Aboslute Y: ");
        Serial.println(value);
      } else if (strcmp(action, "RX") == 0) {
        Serial.print("Relative X: ");
        Serial.println(value);
      } else if (strcmp(action, "RY") == 0) {
        Serial.print("Relative Y: ");
        Serial.println(value);
      }
    } else if (type[0] == 'D') { // DHT message
      if (strcmp(action, "GTemp") == 0) {
        Serial.print("DHT_Temp:");
        Serial.println(dht.readTemperature());
      } else if (strcmp(action, "GHumi") == 0) {
        Serial.print("DHT_Humi:");
        Serial.println(dht.readHumidity());
      }
    } else if (type[0] == 'W') {
      if (strcmp(action, "color") == 0) {
        String value2string(value);
        int i1 = value2string.indexOf(',');
        int i2 = value2string.indexOf(',', i1 + 1);

        int R = value2string.substring(0, i1).toInt();
        int G = value2string.substring(i1 + 1, i2).toInt();
        int B = value2string.substring(i2 + 1).toInt();
        leds[0] = CRGB(R, G, B);
        FastLED.show();

        Serial.print("WS2812B ");
        Serial.print(R);
        Serial.print(",");
        Serial.print(G);
        Serial.print(",");
        Serial.println(B);
      }
    } else if (type[0] == 'O') {
      if (strcmp(action, "print") == 0) {
        oled.print(value);
        Serial.print("print ");
        Serial.println(value);
      }
      if (strcmp(action, "println") == 0) {
        oled.println(value);
        Serial.print("println ");
        Serial.println(value);
      }
      if (strcmp(action, "clear") == 0) {
        oled.clear();
        Serial.println("Clearing OLED");
      }
    } // TODO read battery level
  }
}
