#include "ROSIE.h"
#include "ROSIEBLEServerCallbacks.h"
#include <Arduino.h>

ROSIE::ROSIE()
{
#ifdef __DEBUG__
    Serial.begin(115200);
#endif

    motor_manager = new MotorManager((MotorManagerParams){.cnt = 2, .speed_ROC_delay = 40});

    // Pull up resistor for the lever
    // LOW -> ON | HIGH -> OFF
    pinMode(LEVER, INPUT_PULLUP);

    // Init the two motors
    motor_manager->AppendMotor((MotorParams){.PWM_CHANNEL = 0, .PWM = 4, .L_EN = 16, .R_EN = 17});
    motor_manager->AppendMotor((MotorParams){.PWM_CHANNEL = 1, .PWM = 14, .L_EN = 27, .R_EN = 26});

    // Set the initial speed for the two motors
    std::pair<uint8, uint8> motor_speeds = GetMotorSpeedsByPercent(INIT_MOTOR_SPEED_PERCENT);
    motor_manager->SetMaxSpeed(0, motor_speeds.first);
    motor_manager->SetMaxSpeed(1, motor_speeds.second);

    // Update member values for characteristics
    motor_speed_percent = INIT_MOTOR_SPEED_PERCENT;
    motor_1_RPM = MotorSpeedToRPM(motor_speeds.first);
    motor_2_RPM = MotorSpeedToRPM(motor_speeds.second);

    // Create new BLE server
    BLE_profile = new BLEProfile("R.O.S.I.E MK1");
    BLE_profile->SetCallback(new ROSIEServerCallbacks());

    // Create BLE service for the app
    BLE_profile->CreateService("app", APP_UUID);

    // Add characteristics to the app's BLE service
    BLE_profile->AppendCharacteristic(
        "app", "motor_speed_percent", MOTOR_SPEED_PERCENT_UUID, 
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_WRITE_NR | 
        BLECharacteristic::PROPERTY_NOTIFY);
    BLE_profile->AppendCharacteristic(
        "app", "motor_1_RPM", MOTOR_1_RPM_UUID, 
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_NOTIFY);
    BLE_profile->AppendCharacteristic(
        "app", "motor_2_RPM", MOTOR_2_RPM_UUID, 
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_NOTIFY);
    BLE_profile->AppendCharacteristic(
        "app", "ball_spin", BALL_SPIN_UUID, 
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_WRITE_NR | 
        BLECharacteristic::PROPERTY_NOTIFY);

    // Start the app service and begin advertising the service
    BLE_profile->StartService("app");
    BLE_profile->AdvertiseService("app");
    BLE_profile->StartAdvertising();

    // Set the characteristic values
    BLE_profile->SetCharacteristicVal("app", "motor_speed_percent", std::to_string(motor_speed_percent));
    BLE_profile->SetCharacteristicVal("app", "motor_1_RPM", std::to_string(motor_1_RPM));
    BLE_profile->SetCharacteristicVal("app", "motor_2_RPM", std::to_string(motor_2_RPM));
    BLE_profile->SetCharacteristicVal("app", "ball_spin", ball_spin ? "1" : "0");
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
    motor_speed_percent = std::stoi(BLE_profile->GetCharacteristicVal("app", "motor_speed_percent"));
    ball_spin = BLE_profile->GetCharacteristicVal("app", "ball_spin") == "1" ? true : false;
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
    BLE_profile->SetCharacteristicVal("app", "motor_1_RPM", std::to_string(motor_1_RPM));
    BLE_profile->SetCharacteristicVal("app", "motor_2_RPM", std::to_string(motor_2_RPM));

    // // Notify any changes
    if (BLE_profile->GetServiceState("app"))
    {
        BLE_profile->NotifyCharacteristic("app", "motor_speed_percent");
        BLE_profile->NotifyCharacteristic("app", "motor_1_RPM");
        BLE_profile->NotifyCharacteristic("app", "motor_2_RPM");
        BLE_profile->NotifyCharacteristic("app", "ball_spin");
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