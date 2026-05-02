#ifndef ROSIE_BLE_SERVER_CALLBACKS
#define ROSIE_BLE_SERVER_CALLBACKS

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Arduino.h>

class ROSIEServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* server) override
    {
        Serial.println("CLIENT CONNECTED");
    }

    void onDisconnect(BLEServer* server) override
    {
        Serial.println("CLIENT DISCONNECTED");
        BLEDevice::startAdvertising();
    }
};

#endif