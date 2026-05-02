#ifndef BLEPROFILE_H
#define BLEPROFILE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <unordered_map>
#include <unordered_set>
#include "Utilities.h"
#include "BLEServiceWrapper.h"

class BLEProfile
{
public:
    BLEProfile() {}
    BLEProfile(const std::string& name);
    ~BLEProfile() {}

    void SetCallback(BLEServerCallbacks* cb) { server->setCallbacks(cb); }

    void CreateService(const std::string& name, const std::string& UUID)
        { services[name] = new BLEServiceWrapper(server, UUID); }
    void StartService(const std::string& service) { services[service]->Start(); }
    void StopService(const std::string& service) { services[service]->Stop(); }
    void AdvertiseService(const std::string& service) { advertised_services.insert(services[service]); }
    void StartAdvertising();
    void StopAdvertising() { BLEDevice::stopAdvertising(); }
    bool GetServiceState(const std::string& service) { return services[service]->GetState(); }

    void AppendCharacteristic(const std::string& service, const std::string& name, const std::string& UUID, uint32 properties)
        { services[service]->AppendCharacteristic(name, UUID, properties); }
    void SetCharacteristicVal(const std::string& service, const std::string& name, const std::string& val)
        { services[service]->SetCharacteristicVal(name, val); }
    std::string GetCharacteristicVal(const std::string& service, const std::string& name)
        { return services[service]->GetCharacteristicVal(name); }
    void NotifyCharacteristic(const std::string& service, const std::string& name)
        { services[service]->NotifyCharacteristic(name); }
private:
    std::string name;
    BLEServer* server = nullptr;

    // Services
    std::unordered_map<std::string, BLEServiceWrapper*> services;

    // Advertising the services
    BLEAdvertising* advertising = nullptr;
    std::unordered_set<BLEServiceWrapper*> advertised_services;
};

#endif