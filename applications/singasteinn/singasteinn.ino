#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LEDCollection.h>
#include <FS.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

const int firstLed = 1;
const int nLED = 12;
JollyGood::NLEDCollection<nLED> leds;

void handleRoot();
void handleNotFound();

long iMode = 0;
CRGB target[nLED];
CRGB start[nLED];
int startTicks[nLED];
int tick[nLED];

const int ms_per_tick = 25;
const int minTicks = 40;  // 1s
const int maxTicks = 400; // 10s
const int newLightRate = 400;  // per 10s.
const int offRate = 30;  // per 2s.

const CRGB colorsMode0[] = {CRGB::Blue, CRGB::Green, CRGB::Aquamarine, CRGB::Aqua};
const CRGB colorsMode1[] = {CRGB::White, CRGB::Goldenrod, CRGB::LightGoldenrodYellow, CRGB::LightGoldenrodYellow, CRGB::Ivory};
const CRGB *colors[] = {colorsMode0, colorsMode1};
const int nModes = 2;
const int nColors[] = {sizeof(colorsMode0)/sizeof(CRGB), sizeof(colorsMode1)/sizeof(CRGB)};

void setup() {
  Serial.begin(115200);
  Serial.println("Singasteinn, by Dave Barnett");
  Serial.println("dave.barnett@jollygood.org.uk");
  
  WiFi.softAP("singasteinn", "culhaven");

  if (!MDNS.begin("singasteinn")) {
    Serial.println("Error setting up MDNS responder!");
  }

  leds.Setup<WS2812B, 2, GRB>();
  leds[0] = CRGB::Green;
  for (int i = firstLed; i < nLED; i++)
  {
    leds[i] = CRGB::Blue;
    start[i] = leds[i];
    startTicks[i] = random(minTicks, maxTicks);
    tick[i] = startTicks[i];
  }
  FastLED.show();

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

CRGB targetColor(int i) {
  if (leds[i].r > 0 || leds[i].g > 0 || leds[i].b > 0) {
    return CRGB::Black;
  } else if (iMode < nModes) {
    int n = random(nColors[iMode]);
    return colors[iMode][n];
  }
  else {
    return CRGB::White;
  }
}

void loop() {
  server.handleClient();

  EVERY_N_MILLISECONDS(ms_per_tick)
  {
    for (int i = firstLed; i < nLED; i++)
    {
      if (tick[i] == 0)
      {
        // On time and off time are different
        if (random(leds[i] ? offRate : newLightRate) == 1) {
          // Start new transition
          startTicks[i] = random(minTicks, maxTicks);
          tick[i] = startTicks[i];
          target[i] = targetColor(i);
          start[i] = leds[i];
        }
      } else {
        tick[i]--;
        leds[i] = CRGB((start[i].r * tick[i] + target[i].r * (startTicks[i] - tick[i])) / startTicks[i],
                      (start[i].g * tick[i] + target[i].g * (startTicks[i] - tick[i])) / startTicks[i],
                      (start[i].b * tick[i] + target[i].b * (startTicks[i] - tick[i])) / startTicks[i]);
      }
    }
   FastLED.show();
  }
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

void handleRoot() {
  handleFileRead("index.html");
}

void handleNotFound()
{
  if(server.uri()[1] == '_')
  {
    server.send(200, "text/plain", "{DONE}");
    iMode = strtol(server.uri().c_str()+2, NULL, 10);
    if (iMode < 0) {
      iMode = 0;
    }
    if (firstLed > 0) {
      if (iMode < nModes) {
        leds[0] = colors[iMode][0];
      } else {
        leds[0] = CRGB::Red;
      }
    }
    FastLED.show();
  }
  else if (!handleFileRead(server.uri()))
  {
    server.send(404, "text/plain", "404: Stone not found.");
  }
}

