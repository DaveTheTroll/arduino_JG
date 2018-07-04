#include <Flame.h>

const int N = 40;
JollyGood::NLEDCollection<N> leds;
JollyGood::Flame<N> flame(leds,
            45, // sparking
            8, // cooling
            700/60 // period
            );
const int N2 = 40;
JollyGood::NLEDCollection<N2> leds2;
JollyGood::Flame<N2> flame2(leds2,
            45, // sparking
            8, // cooling
            700/60 // period
            );

void setup()
{
  Serial.begin(250000);
  leds.Setup<WS2812B, 6, GRB>();
  leds2.Setup<WS2812B, 5, GRB>();
  flame.hueShift = 0;
  flame.hueShift = 50;
  Serial.print("\r\nSetup Done (X)");
}

void loop()
{
  flame.Tick();
  flame2.Tick();
  FastLED.show();
}
