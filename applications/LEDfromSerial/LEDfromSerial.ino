#include <LEDCollection.h>

const int N = 8;
JollyGood::NLEDCollection<N> leds;

int prev = 0;

void setup()
{
  leds.Setup<WS2811, 5, GRB>();
  Serial.begin(250000);
  for(int i=0; i<N; i++)
        leds[i] = CRGB::Black;
  FastLED.setBrightness(255);
  FastLED.show();
  Serial.print("setup\n");
}

int state = 0;
int n, r, g, b;
void loop()
{
  int j = Serial.available();
  if (j > 0)
  {
    if (millis() - prev > 1000) state = 0;
    Serial.print(state);Serial.print(":");Serial.print(j);Serial.print("\n");

    switch(state)
    {
      case 0:
        n = Serial.parseInt();
        Serial.print(">");Serial.print(n);Serial.print("\n");
        if (n > 0)
          state++;
        break;
      case 1:
      {
        int divider = Serial.read();
        state = (divider == ':') ? state+1 : 0;
        break;
      }
      case 3:
      case 5:
      {
        int divider = Serial.read();
        state = (divider == ',') ? state+1 : 0;
        break;
      }
      case 2:
        r = Serial.parseInt();
        state++;
        break;
      case 4:
        g = Serial.parseInt();
        state++;
        break;
      case 6:
        b = Serial.parseInt();
        if (n <= N &&
          r >= 0 && r <= 255 &&
          g >= 0 && g <= 255 &&
          b >= 0 && b <= 255)
        {
          leds[n-1] = CRGB(r,g,b);
          FastLED.show();
        }
        state=0;
        break;
    }
    prev = millis();
  }
}
