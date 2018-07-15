
#include <LEDCollection.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World Count, by Dave Barnett");
  Serial.println("dave.barnett@jollygood.org.uk");
}

int i = 0;
void loop() {
  EVERY_N_MILLISECONDS(250)
  {
    Serial.println(i++);
  }
}
