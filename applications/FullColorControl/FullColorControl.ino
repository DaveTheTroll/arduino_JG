#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LEDCollection.h>
#include <FS.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

const int nLED = 12;
JollyGood::NLEDCollection<nLED> leds;

void handleRoot();
void handleNotFound();

void setup() {
  Serial.begin(115200);
  Serial.println("Full Color Control, by Dave Barnett");
  Serial.println("dave.barnett@jollygood.org.uk");

  SPIFFS.begin();
  
  WiFi.softAP("singasteinn", "culhaven");

  if (!MDNS.begin("singasteinn")) {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  
  leds.Setup<WS2812B, 2, GRB>();
  for (int i = 0; i < nLED; i++) {
    leds[i] = CRGB::White;
  }
  FastLED.show();
}

void loop() {
  server.handleClient();
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
    int color = strtol(server.uri().c_str()+2, NULL, 16);
    int led = strtol(server.uri().c_str()+9, NULL, 10);
    Serial.print("LED: ");Serial.print(led);
    Serial.print("  Color: ");Serial.print(color);
    Serial.print("  (");Serial.print((color & 0xFF0000) >> 16);
    Serial.print(", ");Serial.print((color & 0xFF00) >> 8);
    Serial.print(", ");Serial.print(color & 0xFF);
    Serial.println(")");
    
    if (led >= 0 && led <= nLED) {
      leds[led] = CRGB((color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF);
    }
    FastLED.show();
  }
  else if (!handleFileRead(server.uri()))
  {
    server.send(404, "text/plain", "404: Stone not found.");
  }
}
