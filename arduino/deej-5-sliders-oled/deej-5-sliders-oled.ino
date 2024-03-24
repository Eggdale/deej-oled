#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A6};

int displayVolume[NUM_SLIDERS];
int analogSliderValues[NUM_SLIDERS];

const unsigned long sleepAfter = 15000; // this value will change how long the oled will display until turning off.
unsigned long startTime;
unsigned long currTime;

//Display Stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//see http://javl.github.io/image2cpp/ for how to make these
const unsigned char discord [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x3f, 0xfe, 0x00, 0x3f, 
  0xfe, 0x00, 0x3f, 0xfe, 0x00, 0xf1, 0xc7, 0x80, 0xf1, 0xc7, 0x80, 0xf1, 0xc7, 0x80, 0xf1, 0xc7, 
  0x80, 0xff, 0xff, 0x80, 0xff, 0xff, 0x80, 0x3c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char steam [] PROGMEM = {
  0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x0f, 0xfc, 0x00, 0x1f, 0xfe, 0x00, 0x3f, 0xc3, 0x00, 0x3f,
  0x81, 0x80, 0x7f, 0x99, 0x80, 0x7f, 0x19, 0x80, 0x7f, 0x03, 0x80, 0x1e, 0x03, 0x80, 0x00, 0x1f,
  0x80, 0x00, 0x3f, 0x80, 0x30, 0xff, 0x80, 0x38, 0xff, 0x00, 0x1f, 0xfe, 0x00, 0x0f, 0xfc, 0x00,
  0x07, 0xf8, 0x00, 0x00, 0x00, 0x00
};

const unsigned char sys [] PROGMEM = {
  0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff,
  0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x3f,
  0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0,
  0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xc0
};

const unsigned char music [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07,
  0xff, 0x00, 0x07, 0xe3, 0x00, 0x06, 0x03, 0x00, 0x04, 0x03, 0x00, 0x04, 0x03, 0x00, 0x04, 0x03,
  0x00, 0x04, 0x1f, 0x00, 0x0c, 0x3f, 0x00, 0x7c, 0x3e, 0x00, 0x7c, 0x3c, 0x00, 0x7c, 0x00, 0x00,
  0x78, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char browser [] PROGMEM = {
  0x00, 0x00, 0x00, 0x40, 0x0c, 0x00, 0x62, 0x07, 0x00, 0x7e, 0x03, 0x80, 0x7f, 0x81, 0x80, 0x7f, 
  0x01, 0xc0, 0xfe, 0x01, 0xc0, 0xfe, 0x01, 0xc0, 0xfc, 0x03, 0xc0, 0xfe, 0xe3, 0xc0, 0x7f, 0xc3, 
  0xc0, 0x7f, 0x07, 0x80, 0x7f, 0x9f, 0x80, 0x3f, 0xff, 0x00, 0x1f, 0xfe, 0x00, 0x0f, 0xfc, 0x00, 
  0x03, 0xf0, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  startTime = millis();
  
}

void loop() {
  currTime = millis();
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  if (currTime - startTime >= sleepAfter) {
    display.clearDisplay();
    display.display();
    startTime = currTime;
  } 
  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    analogSliderValues[i] = analogRead(analogInputs[i]);

    if (analogSliderValues[i] > displayVolume[i] + 10 || analogSliderValues[i] < displayVolume[i] - 10) {
      displayVolume[i] = analogSliderValues[i];
      displayVol(i);
      display.display();
      startTime = currTime;
    }

  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
}

void displayVol(int i) {
  //draw lines and appropriate icon
  display.clearDisplay();
  int disvol = displayVolume[i]  / 10.22;
  display.fillRect(30, 20, map(displayVolume[i], 0, 1023, 0, 60), 5,  SSD1306_WHITE);
  switch (i){
    case 0:
      display.drawBitmap(7, 2, sys, 18, 18, WHITE);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(30,0);             // Start at top-left corner
      display.println("System:");
      display.setCursor(91,18);
      display.setTextSize(1); 
      display.println(disvol);  
    break;
    case 1:
      display.drawBitmap(7, 2, discord, 18, 18, WHITE);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(30,0);             // Start at top-left corner
      display.println("Discord:");  // Edit display text here
      display.setCursor(91,18); 
      display.setTextSize(1);
      display.println(disvol);
    break;
    case 2:
      display.drawBitmap(7, 2, browser, 18, 18, WHITE);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(30,0);             // Start at top-left corner
      display.println("Browser:");  // Edit display text here
      display.setCursor(91,18);
      display.setTextSize(1);  
      display.println(disvol);
    break;
    case 3:
      display.drawBitmap(7, 2, music, 18, 18, WHITE);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(30,0);             // Start at top-left corner
      display.println("Music:");  // Edit display text here
      display.setCursor(91,18); 
      display.setTextSize(1);
      display.println(disvol);
    break;
    case 4:
      display.drawBitmap(7, 2, steam, 18, 18, WHITE);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(30,0);             // Start at top-left corner
      display.println("Game:");  // Edit display text here
      display.setCursor(91,18); 
      display.setTextSize(1);
      display.println(disvol);
    break;
  }
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}
