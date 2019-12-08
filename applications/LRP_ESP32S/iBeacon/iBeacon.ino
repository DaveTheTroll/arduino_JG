#include "BLEDevice.h"
#include "BLEBeacon.h"
#include "Settings.h"

BLEAdvertising *pAdvertising;

void create_beacon() {
  BLEBeacon beacon;
  beacon.setManufacturerId(manufacturer_id);
  beacon.setProximityUUID(BLEUUID(beacon_uuid));
  beacon.setMajor(0xBB);
  beacon.setMinor(0xCC);
  
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
  Serial.print("Jolly Good iBeacon");

  BLEDevice::init("JGiBeacon");
  pAdvertising = BLEDevice::getAdvertising();
  create_beacon();
  pAdvertising->start();
  delay(100);
  pAdvertising->stop();
  esp_deep_sleep(1000000LL * DEEP_SLEEP_DURATION);
}

void loop() {
}
