
#include "LEDCollection.h"
const int N = 80;
JollyGood::NLEDCollection<N+1> leds;

const int bpm = 60;
const float dicotic_notch = 0.9f;

const int elongation_n = 180;
const int elongation_d = 100;

const float pi = 3.1415927f;

void setup() {
  Serial.begin(250000);
  leds.Setup<WS2812B, 6, GRB>();
  Serial.print("\r\nHeart\r\n");
}

void loop() {
  unsigned long now = millis();

  float t = ((float)((now * elongation_n * bpm / 60000) % elongation_n)) / elongation_d;

  float phi = (t - 0.15) * 2 * pi;
  float a = sin(phi) + 0.9*sin(2*phi);
  
  if (a < 0 || t > 1) a = 0;
  const float a_max = 3;
  
  uint8_t R = (uint8_t)(16 + 64 * a / a_max);
  
  for(int n=0; n<N; n++)
  {
    leds[n].setRGB(R, 0, 0);
  }
  
  FastLED.show();
}
