#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LEDCollection.h>
#include <FS.h>
#include <ESP8266mDNS.h>

#define UNDERBRIDGE  1
#define ACCESS_POINT 0

ESP8266WebServer server(80);
void handleRoot();
void handleNotFound();

const int nLED = 12;
JollyGood::NLEDCollection<nLED> leds;

int blinkPeriod = 6000;
int closeTime = 100;
int offTime = 100;
int openTime = 100;

CRGB color = CRGB::Red;

void setup()
{
  Serial.begin(115200);
  Serial.print("\r\nSetup Done");

  leds.Setup<WS2812B, 2, GRB>();
  for (int i = 0; i < nLED; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

#if UNDERBRIDGE==1
  WiFi.begin("UnderBridgeS2", "billygoat");

  int i = 0;
  Serial.println("UnderBridgeS2");

  String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";
  }
  Serial.println(str);
  Serial.println("...");
  
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
    leds[0] = (i % 2) == 0 ? CRGB::Blue : CRGB::Yellow;
    FastLED.show();
  }
  leds[0] = CRGB::Green;
#elif ACCESS_POINT==1
  WiFi.softAP("singing_stone", "culhaven");
  Serial.println("singing_stone");
  leds[0] = CRGB::Purple;
#endif
  FastLED.show();

  Serial.println("");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("singing_stone")) {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();

  float t = millis() % blinkPeriod;
  float brightness;
  bool change = true;

  Serial.print(t);
  if (t < closeTime)
  {
    brightness = (closeTime - t) / closeTime;
  }
  else if (t < closeTime + offTime)
  {
    brightness = 0;
  }
  else if (t < closeTime + offTime + openTime)
  {
    brightness = (t - closeTime - offTime) / closeTime;
  }
  else
  {
    change = false;
  }

  if (change)
  {
    CRGB dimColor = CRGB((uint8_t)(color.r * brightness), (uint8_t)(color.g * brightness), (uint8_t)(color.b * brightness));
    
    for(int i=0; i<2; i++) {
      leds[i] = dimColor;
    }
    FastLED.show();
  }
}

void handleRoot()
{
  server.send(200, "text/html", "Hello from the singing stone.");
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

void handleNotFound()
{
  if(server.uri()[1] == '_')
  {
    server.send(200, "text/plain", "{DONE}");
    long rgb = strtol(server.uri().c_str()+2, NULL, 16);
    color = CRGB((rgb & 0xF00) >> 4, (rgb & 0xF0), (rgb & 0xF) << 4);
    for(int i=0; i<2; i++) {
      leds[i] = color;
    }
    FastLED.show();
  }
  else if (!handleFileRead(server.uri()))
  {
    server.send(404, "text/plain", "404: Stone not found.");
  }
}


