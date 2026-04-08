#ifndef ROSIE_H
#define ROSIE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "Utilities.h"
#include "MotorManager.h"

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

struct Characteristic
{
    const char* UUID = nullptr;
    BLECharacteristic* characteristic = nullptr;
};

class ROSIE
{
public:
    ROSIE();
    ~ROSIE();

    void Loop();
private:
    std::pair<uint8, uint8> GetMotorSpeedsByPercent(uint percent);
    void BallSpin(bool enable) { ball_spin = enable; }

    uint MotorSpeedToRPM(uint8 motor_speed);
    uint MotorSpeedPercentToRPM(uint percent);
    uint8 MotorSpeedPercentToMotorSpeed(uint percent);
    uint8 RPMToMotorSpeed(uint RPM);

    MotorManager* motor_manager = nullptr;

    // Lever
    const uint8 LEVER = 33;
    int lever_state;

    // Motor and battery
    const float MAX_VOLTAGE = 24.0f;
    const float BATTERY_VOLTAGE = 22.2f;
    const uint MAX_RPM = 20000;

    // Wheels
    const float WHEEL_RADIUS = InchesToMeters(2.0f);
    const float WHEEL_CENTER_SEPARATION = MilimetersToMeters(147.6f);

    // Ball
    const uint BALL_RPM = 2000;
    const float BALL_ANGULAR_VELOCITY = BALL_RPM * 2 * PI / 60.0f;
    const float BALL_RADIUS = 0.0333f;
    const float COMPRESSED_BALL_RADIUS = WHEEL_CENTER_SEPARATION / 2.0f - WHEEL_RADIUS;

    bool ball_spin = true;

    bool first_time_on = true;

    // BLE
    BLEServer* server;
    BLEService* service;
    bool service_ready = false;
    const char* SERVICE_UUID = "c82088ae-79eb-4e15-9e11-87dd59d897d5";

    uint motor_speed_percent;
    uint motor_1_RPM, motor_2_RPM;
    Characteristic motor_speed_percent_char;
    Characteristic motor_1_RPM_char, motor_2_RPM_char;
    Characteristic ball_spin_char;
};

#endif