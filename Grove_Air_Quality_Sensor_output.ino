//sensor to A0 port
//LCD to I2C
//RGB LED stick to D6

#include "Air_Quality_Sensor.h"

#include <Wire.h>
#include "rgb_lcd.h"

AirQualitySensor sensor(A0);
rgb_lcd lcd;

//set brightness 0-255 and colour here
const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//turn on the whole strip at once
void setStripColor(int _quality)
{
  for (uint8_t i = 0; i <= 9; i++)
  {
    if (_quality == AirQualitySensor::FRESH_AIR) strip.setPixelColor(i, strip.Color(0, 255, 0)); //green
    else if (_quality == AirQualitySensor::LOW_POLLUTION) strip.setPixelColor(i, strip.Color(255, 255, 0)); //yellow
    else if (_quality == AirQualitySensor::HIGH_POLLUTION) strip.setPixelColor(i, strip.Color(255, 128, 0)); //orange
    else strip.setPixelColor(i, strip.Color(255, 0, 0)); //red

    strip.show();
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);

  lcd.setCursor(0, 0);
  lcd.print("Initialising ...");
  Serial.println("Waiting sensor to init...");
  delay(2000);

  if (sensor.init()) {
    Serial.println("Sensor ready.");
    lcd.print("Sensor ready");
  }
  else {
    Serial.println("Sensor ERROR!");
    lcd.print("Sensor ERROR!");
  }

  delay(1000);

  strip.begin();
  strip.setBrightness(255);
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void) {
  int quality = sensor.slope();

  lcd.setCursor(0, 0); //1st row
  lcd.print("Sensor value: ");
  Serial.print("Air quality sensor value: ");

  lcd.setCursor(0, 1); //2nd row
  lcd.print(sensor.getValue());
  Serial.println(sensor.getValue());

  if (quality == AirQualitySensor::FORCE_SIGNAL) {
    Serial.println("High pollution! Force signal active.");
    lcd.print(", v high poll");
  }
  else if (quality == AirQualitySensor::HIGH_POLLUTION) {
    Serial.println("High pollution!");
    lcd.print(", high poll");
  }
  else if (quality == AirQualitySensor::LOW_POLLUTION) {
    Serial.println("Low pollution!");
    lcd.print(", low poll");
  }
  else if (quality == AirQualitySensor::FRESH_AIR) {
    Serial.println("Fresh air.");
    lcd.print(", fresh air");
  }

  Serial.println();

  strip.clear();
  strip.show();
  delay(500);

  setStripColor(quality);

  delay(5000);
  lcd.clear();
}
