#include <LEDCollection.h>

const int N = 8;
JollyGood::NLEDCollection<N> leds;

const uint32_t rate = 7;

uint32_t last = 0;
uint8_t currentLed = 0;
uint8_t hueMin = 0*255/8;
uint8_t hueMax = 2*255/8;
uint8_t hueShift = 3*255/8;
uint8_t j = 0;
uint8_t hueTarget = 0;
uint8_t hue[N];

void setup() {
  leds.Setup<WS2811, 5, GRB>();
  for(int i=0; i<N; i++)
  {
    hue[i] = random(hueMin,hueMax);
    leds[i].setHSV(hue[i], 255, 255);
  }
  FastLED.show();
}

void loop()
{
  uint32_t now = millis();
  if (now - last >= rate)
  {
    last += rate;
    if (hue[currentLed] == hueTarget)
    {
      leds[currentLed].setHSV((hueTarget + hueShift + 256)%256, 255, 255);
      currentLed = random(0, N);
      hueTarget = random(hueMin,hueMax);
    }
    else
    {
      hue[currentLed] += (hue[currentLed] > hueTarget) ? -1 : 1;
      leds[currentLed].setHSV((hue[currentLed] + hueShift + 256)%256, 255, 255);
      FastLED.show();
    }
  }
}
