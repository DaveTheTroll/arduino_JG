#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <LEDCollection.h>
#include <FS.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
const int nLED = 12;
JollyGood::NLEDCollection<nLED> leds;

void handleRoot();
void handleNotFound();

int iMode = 1;
const int nActive = 4;
CRGB start[nActive];
CRGB target[nActive];
int currentLed[nActive];
int steps[nActive];
int pos[nActive];
int state[nActive];

const int ms_per_tick = 25;
const int minSteps = 20;  // 0.5s
const int maxSteps = 120; // 3s

const CRGB colorsMode0[] = {CRGB::Blue, CRGB::Green, CRGB(0,255,0), CRGB::Aqua};
const CRGB colorsMode1[] = {CRGB::White, CRGB::Yellow, CRGB::LightGoldenrodYellow, CRGB::Ivory};
const CRGB *colors[] = {colorsMode0, colorsMode1};
const int nColors[] = {sizeof(colorsMode0)/sizeof(CRGB), sizeof(colorsMode1)/sizeof(CRGB)};
const int nModes = 2;

void setup() {
  Serial.begin(115200);
  Serial.println("Singasteinn 2, by Dave Barnett");
  Serial.println("dave.barnett@jollygood.org.uk");

  SPIFFS.begin();
  
  WiFi.softAP("singasteinn2", "culhaven");

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  leds.Setup<WS2812B, 2, GRB>();
  for (int i = 0; i < nLED; i++)
  {
    leds[i] = CRGB::Black;
  }
  for(int n=0; n<nActive; n++)
  {
    currentLed[n] = n;
    steps[n] = 10;
    pos[n] = 0;
    target[n] = colors[iMode][0];
    state[n] = 0;
  }
  FastLED.show();
}

void loop() {
  server.handleClient();

  EVERY_N_MILLISECONDS(ms_per_tick)
  {
    for(int n=0; n<nActive; n++)
    {
      pos[n]++;
      if (pos[n] == steps[n])
      {
        pos[n] = 0;
        steps[n] = random(minSteps, maxSteps);
        start[n] = target[n];
        state[n] = (state[n]+1) % 4;

        int c = -1;
        switch(state[n])
        {
          case 1: // Stay on
          case 3: // Stay off
            break;
          case 2: // Fade to black
            target[n] = CRGB::Black;
            break;
          case 0: // Next LED fase to colour
          default:
            c = random(nColors[iMode]);
            target[n] = (iMode < nModes) ? colors[iMode][c] : CRGB::Black;
            bool clash;
            do
            {
              currentLed[n] = random(nLED);
              clash = false;
              for(int m=0; m<nActive; m++)
              {
                if (m != n && currentLed[n] == currentLed[m])
                  clash = true;  
              }
            } while (clash);
        }

        Serial.print(n); Serial.print(" L");
        Serial.print(currentLed[n]); Serial.print(" S");
        Serial.print(state[n]); Serial.print(" ");
        Serial.print(c); Serial.print(" (");
        Serial.print(target[n].r); Serial.print(", ");
        Serial.print(target[n].g); Serial.print(", ");
        Serial.print(target[n].b); Serial.println(")");
      }
      else
      {
        leds[currentLed[n]] = CRGB((start[n].r * (steps[n]-pos[n]) + target[n].r * pos[n]) / steps[n],
                                    (start[n].g * (steps[n]-pos[n]) + target[n].g * pos[n]) / steps[n],
                                    (start[n].b * (steps[n]-pos[n]) + target[n].b * pos[n]) / steps[n]);
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
  Serial.println("handleRoot");
  handleFileRead("/");
}

void handleNotFound()
{
  Serial.println("404");
  if(server.uri()[1] == '_')
  {
    server.send(200, "text/plain", "{DONE}");
    iMode = strtol(server.uri().c_str()+2, NULL, 10);
    Serial.print("Mode: ");Serial.println(iMode);
    FastLED.show();
  }
  else if (!handleFileRead(server.uri()))
  {
    server.send(404, "text/plain", "404: Stone not found.");
  }
}

