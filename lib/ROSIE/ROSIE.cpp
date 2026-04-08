#include "ROSIE.h"
#include <Arduino.h>

ROSIE::ROSIE()
{
    motor_manager = new MotorManager((MotorManagerParams){.cnt = 2, .speed_ROC_delay = 40});

    // Pull up resistor for the lever
    // LOW -> ON | HIGH -> OFF
    pinMode(LEVER, INPUT_PULLUP);

    // Init the two motors
    motor_manager->AppendMotor((MotorParams){.PWM_CHANNEL = 0, .PWM = 4, .L_EN = 16, .R_EN = 17});
    motor_manager->AppendMotor((MotorParams){.PWM_CHANNEL = 1, .PWM = 14, .L_EN = 27, .R_EN = 26});

    // Set the initial speed for the two motors
    #define INIT_MOTOR_SPEED_PERCENT 25
    std::pair<uint8, uint8> motor_speeds = GetMotorSpeedsByPercent(INIT_MOTOR_SPEED_PERCENT);
    motor_manager->SetMaxSpeed(0, motor_speeds.first);
    motor_manager->SetMaxSpeed(1, motor_speeds.second);

    // Init BLE
    BLEDevice::init("R.O.S.I.E MK1");
    server = BLEDevice::createServer();
    server->setCallbacks(new ROSIEServerCallbacks());
    service = server->createService(SERVICE_UUID);

    motor_speed_percent_char.characteristic = service->createCharacteristic(
        motor_speed_percent_char.UUID = "7e77c276-cd8d-4b29-bfcf-ea1bb488a49b",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE_NR | BLECharacteristic::PROPERTY_NOTIFY);

    motor_1_RPM_char.characteristic = service->createCharacteristic(
        motor_1_RPM_char.UUID = "76bd9c05-8508-4ade-91ea-cab8a83a5cac",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    motor_2_RPM_char.characteristic = service->createCharacteristic(
        motor_2_RPM_char.UUID = "d7bc8ea0-ceed-4534-a6e1-88061f51ae37",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    ball_spin_char.characteristic = service->createCharacteristic(
        ball_spin_char.UUID = "775c47cf-69c4-4ec7-bde0-0e49e75fdebe",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE_NR | BLECharacteristic::PROPERTY_NOTIFY);

    service->start();

    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x12); // Functions that help with iPhone connections issue
    BLEDevice::startAdvertising();

    service_ready = true;

    // Update member values for characteristics
    motor_speed_percent = INIT_MOTOR_SPEED_PERCENT;
    motor_1_RPM = MotorSpeedToRPM(motor_speeds.first);
    motor_2_RPM = MotorSpeedToRPM(motor_speeds.second);

    // Update the characteristic values to match member values
    motor_speed_percent_char.characteristic->setValue(std::to_string(motor_speed_percent));
    motor_1_RPM_char.characteristic->setValue(std::to_string(motor_1_RPM));
    motor_2_RPM_char.characteristic->setValue(std::to_string(motor_2_RPM));
    ball_spin_char.characteristic->setValue(ball_spin ? "1" : "0");

#ifdef __DEBUG__
    Serial.begin(115200);
#endif
}

ROSIE::~ROSIE()
{
    delete motor_manager;
}

void ROSIE::Loop()
{
    // Accelerate or decelerate the motors depending on the switch state
    lever_state = digitalRead(LEVER);

    // Safeguard for in case it is powered on with switch in ON state
    if (lever_state == LOW && first_time_on)
        return;
    else
        first_time_on = false;

    // Update member variables with characteristic values
    ball_spin = ball_spin_char.characteristic->getValue() == "1" ? true : false;
    motor_speed_percent = std::stoi(motor_speed_percent_char.characteristic->getValue());
    std::pair<uint8, uint8> motor_speeds = GetMotorSpeedsByPercent(motor_speed_percent);
    motor_1_RPM = MotorSpeedToRPM(motor_speeds.first);
    motor_2_RPM = MotorSpeedToRPM(motor_speeds.second);

    // Accelerate or decelerate the motors
    if (lever_state == LOW)
    {
        motor_manager->SetMaxSpeed(0, motor_speeds.first);
        motor_manager->SetMaxSpeed(1, motor_speeds.second);
    }
    else
    {
        motor_manager->SetMaxSpeed(0, 0);
        motor_manager->SetMaxSpeed(1, 0);
    }

    // Update the speeds of the motors
    motor_manager->Update();

    // Set characteristic values to be updated member values
    motor_1_RPM_char.characteristic->setValue(std::to_string(motor_1_RPM));
    motor_2_RPM_char.characteristic->setValue(std::to_string(motor_2_RPM));

    // Notify any changes
    if (service_ready)
    {
        motor_speed_percent_char.characteristic->notify();
        motor_1_RPM_char.characteristic->notify();
        motor_2_RPM_char.characteristic->notify();
        ball_spin_char.characteristic->notify();
    }
}

std::pair<uint8, uint8> ROSIE::GetMotorSpeedsByPercent(uint percent)
{
    // Motor 1
    uint motor_1_RPM = MotorSpeedPercentToRPM(percent);
    float motor_1_angular_velocity = RPMToAngularVelocity(motor_1_RPM);

    // Linear velocity of the ball
    float linear_velocity = 
        motor_1_angular_velocity * WHEEL_RADIUS - BALL_ANGULAR_VELOCITY * COMPRESSED_BALL_RADIUS;

    // Motor 2
    uint motor_2_RPM = motor_1_RPM;
    if (ball_spin)
    {
        float motor_2_angular_velocity = 2 * linear_velocity / WHEEL_RADIUS - motor_1_angular_velocity;
        motor_2_RPM = AngularVelocityToRPM(motor_2_angular_velocity);
    }

    // Set the speeds
    return std::make_pair(RPMToMotorSpeed(motor_1_RPM), RPMToMotorSpeed(motor_2_RPM));
}

uint ROSIE::MotorSpeedToRPM(uint8 motor_speed) 
{
    return (uint)((motor_speed / 255.0f) * (BATTERY_VOLTAGE / MAX_VOLTAGE) * MAX_RPM);
}

uint ROSIE::MotorSpeedPercentToRPM(uint percent)
{
    if (percent > 100.0f || percent < 0.0f)
    {
#ifdef __DEBUG__
        Serial.println("ROSIE::SpeedPercentToRPM | Percent not within 0-100");
#endif
        return 0.0f;
    }
    return (uint)(percent / 100.0f * MAX_RPM * BATTERY_VOLTAGE / MAX_VOLTAGE);
}

uint8 ROSIE::MotorSpeedPercentToMotorSpeed(uint percent)
{
    if (percent > 100.0f || percent < 0.0f)
    {
#ifdef __DEBUG__
        Serial.println("ROSIE::SpeedPercentToMotorSpeed | Percent not within 0-100");
#endif
        return 0.0f;
    }
    return (uint8)(percent / 100.0f * 255.0f);
}

uint8 ROSIE::RPMToMotorSpeed(uint RPM)
{
    return (uint8)(RPM / (MAX_RPM * BATTERY_VOLTAGE / MAX_VOLTAGE) * 255.0f);
}