#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <FastLED.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

/* OLED */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* DHT */
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* MOTORES */
#define EN 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
int driverPins[] = {EN, IN1, IN2, IN3, IN4};

/* SERVOS */
#define servoX 7
#define servoY 8
Servo servoXaxis;
Servo servoYaxis;

/* LED */
#define LED_PIN 7
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
/* Battery */
#define BATTERY_PIN A0

const int MAX_INPUT_LENGTH = 256;   // Define the maximum input length
char serialInput[MAX_INPUT_LENGTH]; // Character array for serial input
int serialIndex = 0;

void setup() {
  int numPins = sizeof(driverPins) / sizeof(driverPins[0]);
  for (int i = 0; i < numPins; i++) {
    pinMode(driverPins[i], OUTPUT);
  }
  pinMode(BATTERY_PIN, INPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  Serial.begin(9600);

  dht.begin();

  servoXaxis.attach(servoX);
  servoYaxis.attach(servoY);

  /*
  while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  */
  display.clearDisplay();
  display.display();
}

void loop() {
  if (Serial.available()) {
    char receivedChar = Serial.read();
    if (receivedChar == '\n') {
      serialInput[serialIndex] = '\0';
      processMessage(serialInput);
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

  if (sscanf(message, "%1s:%[^=]=%[^\n]", type, action, value) == 3) {
    Serial.println(message);
    switch (type[0]) {
    case 'M': // Motor
      motorHandle(action, value);
      break;
    case 'S': // Servo
      servoHandle(action, value);
      break;
    case 'B': // Battery
      batteryHandle(action);
      break;
    case 'L': // ARGB LED (WS2812B) change color depending on battery level
      break;
    case 'D': // DHT
      dhtHandle(action);
      break;
    case 'O': // OLED
      break;
    }
  }
}

void motorHandle(char action, char value) {
  int speed = int(value);
  if (strcmp(action, "forward") == 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (strcmp(action, "backward") == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (strcmp(action, "left") == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (strcmp(action, "right") == 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (strcmp(action, "stop") == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  analogWrite(EN, speed);
}

void servoHandle(char action, char value) {
  int degrees = int(value);
  if (strcmp(action, "x") == 0) {
    servoXaxis.write(value);
  } else if (strcmp(action, "y") == 0) {
    servoYaxis.write(value);
  }
}

void batteryHandle(char action) {
  if (strcmp(action, "get") == 0) {
    int batteryValue = analogRead(BATTERY_PIN);
    Serial.print("Battery: ");
    Serial.println(batteryValue);
  }
}

void dhtHandle(char action) {
  if (strcmp(action, "get") == 0) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
  }
}