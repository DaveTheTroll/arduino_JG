#include <LEDCollection.h>

uint32_t period = 50;

JollyGood::NLEDCollection<8> leds;
uint8_t v[3], h[3], s[3];
uint8_t tv[3], th[3], ts[3];
uint32_t previousTick;

void setup()
{
  Serial.begin(250000);
  leds.Setup<WS2811, 5, GRB>();
  for(int i=0; i<3; i++)
  {
    v[i] = 128;
  }
  previousTick = millis();
  FastLED.setBrightness(255);
}

void loop()
{
     uint8_t now = millis();
      if (now - previousTick > period)
      {
        for(int i=0; i<3; i++)
        {
          if (v[i] == tv[i])
          {
            tv[i] = random(0, 255);
          }
          else
          {
            if (v[i] > tv[i])
            {
              v[i]--;
            }
            else
            {
              v[i]++;
            }
          }
          if (h[i] == th[i])
          {
            th[i] = random(0, 255);
          }
          else
          {
            if (((h[i] - th[i] + 256) % 256) < 128)
            {
              h[i]++;
            }
            else
            {
              h[i]--;
            }
          }
          if (s[i] == ts[i])
          {
            ts[i] = random(196, 255);
          }
          else
          {
            if (s[i] > ts[i])
            {
              s[i]--;
            }
            else
            {
              s[i]++;
            }
          }
        }

        int j = 0;
        leds[0].setHSV(h[j], s[j], v[j]);
        leds[1].setHSV(h[j], s[j], v[j]);
        j++;
        leds[2].setHSV(h[j], s[j], v[j]);
        leds[3].setHSV(h[j], s[j], v[j]);
        leds[4].setHSV(h[j], s[j], v[j]);
        j++;
        leds[5].setHSV(h[j], s[j], v[j]);
        leds[6].setHSV(h[j], s[j], v[j]);
        leds[7].setHSV(h[j], s[j], v[j]);
        FastLED.show();
      }
}
