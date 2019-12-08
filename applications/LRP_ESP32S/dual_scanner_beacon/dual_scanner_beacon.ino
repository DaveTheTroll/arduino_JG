#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include "Settings.h"

class Vibrator
{
public:
  Vibrator()
  {
    ledcSetup(vib_channel, vib_pwm_freq, vib_resolution);
    ledcAttachPin(vib_pin, vib_channel);
  }

  void setRate(int val)
  {
      ledcWrite(vib_channel, val);
  }
private:
  const int vib_pin = 13;
  const int vib_pwm_freq = 5000;
  const int vib_channel = 0;
  const int vib_resolution = 8;
};

Vibrator vibrator;

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
bool beaconFound;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      unsigned short manufacturerId = *((unsigned short*)advertisedDevice.getManufacturerData().data());
      if(advertisedDevice.haveManufacturerData() && manufacturerId == 0xAAAA)
      {
        Serial.printf("\nJG Device: %s ", advertisedDevice.toString().c_str());
        int rssi = advertisedDevice.getRSSI();

        const int min_rssi = -90;
        const int max_rssi = -40;
        const int rssi_diff = max_rssi - min_rssi;
        const int min_vib = 32;
        const int max_vib = 255;
        const int vib_diff = max_vib - min_vib;
        int vib = min_vib + ((rssi - min_rssi) * vib_diff) / rssi_diff;
        
        Serial.printf("RSSI: %d Vib: %d\n", rssi, vib);
        vibrator.setRate(vib);
        beaconFound = true;
      }
      else
      {
        Serial.printf("%04x:", (int)manufacturerId);
      }
    }
};

BLEAdvertising *pAdvertising;

void create_beacon()
{
  BLEBeacon beacon;
  beacon.setManufacturerId(manufacturer_id);
  beacon.setProximityUUID(BLEUUID(beacon_uuid));
  beacon.setMajor(0xBB);
  beacon.setMinor(0xCCCC);
  
  BLEAdvertisementData advertData;
  advertData.setFlags(0x04);  // BR_EDR_NOT_SUPPORTED
  std::string data;
  data += (char)26;   // length
  data += (char)0xff; // Type
  data += beacon.getData();
  advertData.addData(data);
  pAdvertising->setAdvertisementData(advertData);
  
  BLEAdvertisementData scanResponseData;
  pAdvertising->setScanResponseData(scanResponseData);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  
  pAdvertising = BLEDevice::getAdvertising();
  create_beacon();
}

void loop() {
  // put your main code here, to run repeatedly:
  beaconFound = false;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  if (!beaconFound)
  {
    Serial.println("Not found.  Vib: 0");
    vibrator.setRate(0);
  }
  Serial.println("====");
  Serial.printf("Devices found: %d\n", foundDevices.getCount());
  Serial.println("====");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  Serial.println("Advertising");
  pAdvertising->start();
  delay(random(1800, 2200));
  pAdvertising->stop();
  Serial.println("Done Advertising");
}
