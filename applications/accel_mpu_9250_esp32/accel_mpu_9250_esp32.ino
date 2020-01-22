#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Accelerometer Settings
#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

// WiFi settings
#include "ssid.h"
const int udpPort = 49198;

// WiFi Globals
bool connected = false;
WiFiUDP udp;
long int prev;

void setup()
{
  Serial.begin(115200);
  
  connectToWiFi();
  
  Wire.begin();
  i2cwrite(MPU9250_ADDRESS, 29, 0x06);                      // accelerometers low pass at 5Hz
  i2cwrite(MPU9250_ADDRESS, 26, 0x06);                      // gyro low pass at 5Hz
  i2cwrite(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);  // gyro range
  i2cwrite(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);        // accelerometer range
  i2cwrite(MPU9250_ADDRESS, 0x37, 0x02);                    // bypass mode for magnetometers
  i2cwrite(MAG_ADDRESS, 0x0A, 0x16);                        // continuous magnetometer measurements in 16 bits

  prev = millis();
}

int count = 0;

void loop()
{
  long int now = millis();
  if (now - prev < 50)
  {
    return;
  }
  prev = now;
  
  if(connected)
  {
    // Read accelerometer
    uint8_t buf[14];
    i2cread(MPU9250_ADDRESS, 0x3b, sizeof(buf), buf);  // read accelerometer and gyro
    
    // Wait for magnetometer
    uint8_t st1;
    do
    {
      i2cread(MAG_ADDRESS, 0x02, 1, &st1);
    }
    while(!(st1 & 0x01));

    uint8_t mag[7];
    i2cread(MAG_ADDRESS, 0x03, sizeof(mag), mag);

    uint16_t out[14];

    out[0] = (buf[0] << 8) | buf[1];    // ax
    out[1] = (buf[2] << 8) | buf[3];    // ay
    out[2] = (buf[4] << 8) | buf[5];    // az
    out[3] = (buf[8] << 8) | buf[9];    // gx
    out[4] = (buf[10] << 8) | buf[11];  // gy
    out[5] = (buf[12] << 8) | buf[13];  // gz
    out[6] = (buf[6] << 8) | buf[7];    // temperature
    out[7] = (mag[1] << 8) | mag[0];    // mx
    out[8] = (mag[3] << 8) | mag[2];    // my
    out[9] = (mag[5] << 8) | mag[4];    // mz
    *((long int*)(&out[10])) = now;
  
    udp.beginPacket(serverIp, udpPort);
    udp.write((uint8_t*)out, sizeof(out));
    udp.endPacket();
  }
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
      Serial.print("WiFi connected. ");
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


void i2cread(uint8_t address, uint8_t reg, uint8_t count, uint8_t * data)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, count);
  uint8_t j = 0;
  while(Wire.available())
  {
    data[j++] = Wire.read();
  }
}

void i2cwrite(uint8_t address, uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}
