#include <Flame.h>
#include <SerialCommand.h>

CRGB colors[] = {CRGB(0, 0, 0), CRGB(255, 0, 0), CRGB(255, 128, 0), CRGB(255, 255, 0), CRGB(255, 255, 196)};
JollyGood::NLEDCollection<144> leds;
JollyGood::Flame<20> flame(leds, 90, 25, 1000/60);

const char * cmdHandler(const char * cmd);

SoftwareSerial bt(8, 9);
/*JollyGood::DefaultSerialConnection defaultSerial;
JollyGood::SoftwareSerialConnection btSerial(bt);
JollyGood::SerialCommandConnection defaultCmd(defaultSerial, cmdHandler);
JollyGood::SerialCommandConnection btCmd(btSerial, cmdHandler);*/

void setup()
{
  bt.begin(9600);
  Serial.begin(9600);
  leds.Setup<WS2811, 5, GRB>();
  Serial.print("FlameTooth:\r\n");
  bt.print("FlameTooth:\r\n");
}

void loop()
{
//  btCmd.Tick();
//  defaultCmd.Tick();
  int c = bt.read();
  if (c != -1)
    Serial.print((char)c);
  c = Serial.read();
  if (c != -1)
    Serial.print((char)c);

//  flame.Tick();
//  FastLED.show();
}

const char * cmdHandler(const char * cmd)
{
  Serial.print(cmd);
}

