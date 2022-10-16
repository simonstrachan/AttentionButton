//ESP32 C3 Dev Module 
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define switchPin 0

//BLE server name
#define bleServerName "AttentionButton"

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "ffb2aa1a-a8cb-4784-8cec-360a799f609e"

// Button Characteristic and Descriptor

BLECharacteristic buttonCharacteristic("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor buttonDescriptor(BLEUUID((uint16_t)0x2902));

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  // Start serial communication 
  Serial.begin(115200);

  pinMode(switchPin,OUTPUT);

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *bmeService = pServer->createService(SERVICE_UUID);
  bmeService->addCharacteristic(&buttonCharacteristic);
  buttonDescriptor.setValue("OFF");
  buttonCharacteristic.addDescriptor(&buttonDescriptor);

  // Start the service
  bmeService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    if (digitalRead(switchPin) == 1)  {
      buttonCharacteristic.setValue("ON");
      buttonCharacteristic.notify();
      Serial.println("ON");
    } else {
      buttonCharacteristic.setValue("OFF");
      buttonCharacteristic.notify();
      Serial.println("OFF");
    }
  }
  delay(50);
}