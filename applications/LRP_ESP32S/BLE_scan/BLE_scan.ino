/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      Serial.println("====");
      unsigned short manufacturerId = *((unsigned short*)advertisedDevice.getManufacturerData().data());
      if(advertisedDevice.haveManufacturerData() && manufacturerId == 0xAAAA)
      {
        Serial.printf("\nJG Device: %s ", advertisedDevice.toString().c_str());
        Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
      }
      else
      {
        Serial.printf("%04x:", (int)manufacturerId);
      }
      Serial.println("====");
    }
};

void setup() {
  Serial.begin(9600);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.printf("Devices found: %d\n", foundDevices.getCount());
  Serial.println("====");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}
