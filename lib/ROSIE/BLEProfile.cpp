#include "BLEProfile.h"

BLEProfile::BLEProfile(const std::string& name)
    : name(name)
{
    BLEDevice::init(name);
    server = BLEDevice::createServer();
    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x12); // Helps with iPhone connections issue
}

void BLEProfile::StartAdvertising()
{
    for (BLEServiceWrapper* service : advertised_services)
        advertising->addServiceUUID(service->GetUUID());
    BLEDevice::startAdvertising();
}