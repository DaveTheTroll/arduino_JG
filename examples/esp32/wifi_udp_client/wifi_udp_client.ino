#include <WiFi.h>
#include <WiFiUdp.h>

#include "ssid.h"

const int udpPort = 49198;

bool connected = false;
WiFiUDP udp;

void setup()
{
  Serial.begin(115200);
  connectToWiFi();
}

int count = 0;
void loop()
{
  if(connected)
  {
    udp.beginPacket(serverIp, udpPort);
    udp.printf("... %d %.3f", count++, millis()/1000.0);
    udp.endPacket();
  }

  delay(1000);
}

void connectToWiFi()
{
  Serial.println("Connecting to " + String(ssid));
  WiFi.disconnect(true);
  WiFi.onEvent(onWiFi);
  WiFi.begin(ssid, pwd);
}

void onWiFi(WiFiEvent_t event)
{
  switch(event)
  {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("WiFi connected.");
      Serial.println(WiFi.localIP());

      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi disconnected.");
      connected = false;
      break;
  }
}
