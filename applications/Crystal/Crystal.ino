#include <Flame.h>

const int N = 8;
JollyGood::NLEDCollection<N> leds;

CRGB colors[] = {CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::White};
CRGB current[N];
int n;
int a, b;

CRGB colorA, colorB;

const int delayMs = 4;
const int period = 100;

void setup()
{
  Serial.begin(250000);
  leds.Setup<WS2811, 5, GRB>();

  for(int i=0; i<N; i++)
  {
    int j = i % (sizeof(colors)/sizeof(CRGB));
    current[i] = colors[j];
    Serial.print(j);
  }

  n = 0;
    
  a = random(N);
  b = random(N);
  colorA = current[a];
  colorB = current[b];
}

void loop()
{
  if (n == 0)
  {
    Serial.print(a);
    current[a] = colorB;
    current[b] = colorA;
    
    a = random(N);
    b = random(N);
    colorA = current[a];
    colorB = current[b];
  }
  else
  {
    current[a] = CRGB((colorA.r * (period-n) + colorB.r * n)/100,
                    (colorA.g * (period-n) + colorB.g * n)/100,
                    (colorA.b * (period-n) + colorB.b * n)/100);
    current[b] = CRGB((colorB.r * (period-n) + colorA.r * n)/100,
                    (colorB.g * (period-n) + colorA.g * n)/100,
                    (colorB.b * (period-n) + colorA.b * n)/100);
  }

  n = (n+1)%period;
  
  for(int i=0; i<N; i++)
    leds.Set(i, current[i]);
  FastLED.show();

  delay(delayMs);
}
