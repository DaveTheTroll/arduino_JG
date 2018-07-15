#define FASTLED_ALLOW_INTERRUPTS 0
#include <LEDCollection.h>

const int nLED = 2;
JollyGood::NLEDCollection<nLED> leds;

uint8_t hue = 30;
uint8_t sat = 255;
// BLINK
int period_min =  600000/(176-24);  // 17.6 per minute +/0 2.4)
int period_max = 600000/(176+24);
const int defaultRampTime = 20;
int rampTime = defaultRampTime;
int duration_min = 100 - defaultRampTime;  // 100ms
int duration_max = 400 - defaultRampTime;  // 400ms

int state = 0;
unsigned long nextEvent = 2000;

void setup() {
  leds.Setup<WS2812B, 2, GRB>();
  for (int i = 0; i < nLED; i++)
  {
      leds[i] = CHSV(hue, sat, 255);
  }
  FastLED.show();
}

void loop() {
  EVERY_N_MILLISECONDS(1)
  {
    uint8_t val;
    switch(state)
    {
      case 0: // On
        val = 255;
        if (nextEvent == 0)
        {
          state = 1;
          nextEvent = rampTime;
        }
        break;
      case 1: // Turning off
        val = (uint8_t) (nextEvent * 255 / rampTime);
        if (nextEvent == 0)
        {
          state = 2;
          nextEvent = random(duration_min, duration_max);
        }
        break;
      case 2: // Off
        val = 0;
        if (nextEvent == 0)
        {
          state = 3;
          nextEvent = rampTime;
        }
        break;
      case 3: // Turning on
        val = (uint8_t) (255 - nextEvent * 255 / rampTime);
        if (nextEvent == 0)
        {
          state = 0;
          nextEvent = random(period_min, period_max);
        }
        break;
    }

    nextEvent--;
    
    for (int i = 0; i < nLED; i++)
    {
      leds[i] = CHSV(hue, sat, val);
    }
    
    FastLED.show();
  }
}
