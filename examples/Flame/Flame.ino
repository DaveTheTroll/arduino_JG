#include <Flame.h>

const int N = 80;
CRGB colors[] = {CRGB(0, 0, 0), CRGB(255, 0, 0), CRGB(255, 128, 0), CRGB(255, 255, 0), CRGB(255, 255, 196)};
JollyGood::NLEDCollection<N+1> leds;
JollyGood::Flame<N> flame(leds,
            45, // sparking
            8, // cooling
            700/60 // period
            );

void setup()
{
  Serial.begin(250000);
  leds.Setup<WS2812B, 6, GRB>();
  flame.hueShift = 0;
  Serial.print("\r\nSetup Done (X)");
  leds[N] = CRGB::Red;
}

void loop()
{
  flame.Tick();
  FastLED.show();
}
