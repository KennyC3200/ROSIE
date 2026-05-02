#ifndef BLE_SERVICE_WRAPPER_H
#define BLE_SERVICE_WRAPPER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <unordered_map>
#include "BLECharacteristicWrapper.h"

class BLEServiceWrapper
{
public:
    BLEServiceWrapper() {}
    BLEServiceWrapper(BLEServer* const server, const std::string& UUID)
        : UUID(UUID) { service = server->createService(UUID); }
    ~BLEServiceWrapper() {}

    void Start() { service->start(); ready = true; }
    void Stop() { service->stop(); ready = false; }
    void AppendCharacteristic(const std::string& name, const std::string& UUID, uint32 properties)
        { 
            characteristics[name].UUID = UUID;
            characteristics[name].characteristic = service->createCharacteristic(UUID, properties); 
        }
    void SetCharacteristicVal(const std::string& name, const std::string& val)
        { characteristics[name].characteristic->setValue(val); }
    std::string GetCharacteristicVal(const std::string& name)
        { return characteristics[name].characteristic->getValue(); }
    void NotifyCharacteristic(const std::string& name) 
        { characteristics[name].characteristic->notify(); }
    
    bool GetState() { return ready; }
    const std::string& GetUUID() const { return UUID; }
private:
    BLEService* service = nullptr;

    bool ready = false;
    std::string UUID;

    std::unordered_map<std::string, BLECharacteristicWrapper> characteristics;
};

#endif