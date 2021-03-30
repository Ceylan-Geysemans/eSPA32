#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>


volatile byte state = LOW;

char auth[] = "JpA7T5Rl44OqH7ViUPBRrS6nT3gdyUln";
char ssid[] = "thuisdus";
char pass[] = "Ceylan07";

#define button1 34 //jets1
#define button2 35 //jets2
#define button3 32 //leds
#define button4 33 //temp +
#define button5 25 //temp -

#define heater 5
#define circulation 18
#define jets1 19
#define jets2 23

WidgetLED vheater(V0);
WidgetLED vcirculation(V1);
WidgetLED vjets1(V2);
WidgetLED vjets2(V3);

int switchStatusLast = LOW;  // last status switch
int LEDStatus = LOW;         // current status LED

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define NEOPIN        4 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 6 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);

// GPIO where the DS18B20 is connected to
const int oneWireBus = 13;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


float temperature, setval;

void setup() {

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  pixels.clear(); // Set all pixel colors to 'off'

  display.clearDisplay();

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);

  pinMode(heater, OUTPUT);
  pinMode(circulation, OUTPUT);
  pinMode(jets1, OUTPUT);
  pinMode(jets2, OUTPUT);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 0, 132), 8080);

  //attachInterrupt(digitalPinToInterrupt(button3), toggles, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(button4), toggles, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(button5), toggles, CHANGE);

}

void loop() {
  Blynk.run();
  gettemp();
  getpushbutton();
  printval();
  thermostat();
  toggles();
}

void gettemp() {
  //temperature = sensors.getTempCByIndex(0);
  temperature = 12;
  Blynk.virtualWrite(V4, sensors.getTempCByIndex(0));
}

void getpushbutton() {
  if (digitalRead(button5) == HIGH) {
    setval += 1;
  }
  if (digitalRead(button4) == HIGH) {
    setval -= 1;
  }
}

void thermostat() {
  if (setval >= temperature) {
    digitalWrite(heater, HIGH);
    vheater.on();
  }
  else {
    digitalWrite(heater, LOW);
    vheater.off();
  }
}

void printval() {
  sensors.requestTemperatures();
  display.clearDisplay();
  display.setTextSize(2.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Tem: ");
  display.print(temperature);
  display.setCursor(0, 18);
  display.print("Set: ");
  display.println(setval);
  display.display();
}

void toggles() {
  int switchStatus = digitalRead(button1);   // read status of switch
  if (switchStatus != switchStatusLast)  // if status of button has changed
  {
    // if switch is pressed than change the LED status
    if (switchStatus == HIGH && switchStatusLast == LOW) LEDStatus = ! LEDStatus;
      // turn the LED on or off

    if (LEDStatus == HIGH) {
      vjets1.on();
      digitalWrite(jets1, HIGH);
    }
    else {
      vjets1.off();
      digitalWrite(jets1, LOW);
    }

    switchStatus = switchStatusLast;
  }
}
