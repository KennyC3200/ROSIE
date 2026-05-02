#ifndef BLE_CHARACTERISTIC_WRAPPER_H
#define BLE_CHARACTERISTIC_WRAPPER_H

#include <BLEService.h>
#include "Utilities.h"

struct BLECharacteristicWrapper
{
    std::string UUID;
    BLECharacteristic* characteristic;
};

#endif