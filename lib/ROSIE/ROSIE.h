#ifndef ROSIE_H
#define ROSIE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "Utilities.h"
#include "MotorManager.h"
#include "BLEProfile.h"

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
    const int INIT_MOTOR_SPEED_PERCENT = 25;
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
    BLEProfile* BLE_profile = nullptr;
    const std::string APP_UUID = "c82088ae-79eb-4e15-9e11-87dd59d897d5";
    const std::string MOTOR_SPEED_PERCENT_UUID = "7e77c276-cd8d-4b29-bfcf-ea1bb488a49b";
    const std::string MOTOR_1_RPM_UUID = "76bd9c05-8508-4ade-91ea-cab8a83a5cac";
    const std::string MOTOR_2_RPM_UUID = "d7bc8ea0-ceed-4534-a6e1-88061f51ae37";
    const std::string BALL_SPIN_UUID = "775c47cf-69c4-4ec7-bde0-0e49e75fdebe";
    uint motor_speed_percent;
    uint motor_1_RPM, motor_2_RPM;
};

#endif