#include <Flame.h>

const int N = 80;
CRGB colors[] = {CRGB(0, 0, 0), CRGB(255, 0, 0), CRGB(255, 128, 0), CRGB(255, 255, 0), CRGB(255, 255, 196)};
JollyGood::NLEDCollection<N+1> leds;
JollyGood::Flame<N> flame(leds,
            45, // sparking
            8, // cooling
            700/60 // period
            );

const unsigned long hueChangePeriod = 100;
unsigned long nextHueChange;
void setup()
{
  Serial.begin(250000);
  leds.Setup<WS2812B, 6, GRB>();
  flame.hueShift = 0;
  Serial.print("\r\nSetup Done (X)");
  leds[N] = CRGB::Red;
  nextHueChange = 0;
}

void loop()
{
  unsigned long now = millis();
  if (now - nextHueChange > hueChangePeriod)
  {
    flame.hueShift++;
    Serial.println(flame.hueShift);
    nextHueChange += hueChangePeriod;
  }
  flame.Tick();
  FastLED.show();
}
