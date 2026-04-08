#include "Motor.h"
#include <Arduino.h>

Motor::Motor() {}

Motor::Motor(MotorParams params)
    : PWM_CHANNEL(params.PWM_CHANNEL)
    , PWM(params.PWM)
    , L_EN(params.L_EN)
    , R_EN(params.R_EN)
    , speed(0)
    , max_speed(0)
{
    ledcSetup(PWM_CHANNEL, 5000, 8);
    ledcAttachPin(PWM, PWM_CHANNEL);

    pinMode(L_EN, OUTPUT);
    pinMode(R_EN, OUTPUT);
    digitalWrite(L_EN, HIGH); // Toggle motor turn LEFT
    digitalWrite(R_EN, HIGH); // Toggle motor turn RIGHT
}

Motor::~Motor() 
{
    // Guarantee to turn off the motor
    ledcWrite(PWM_CHANNEL, 0);
}

void Motor::Update()
{
    if (speed < max_speed)
        speed++;
    else if (speed > max_speed)
        speed--;
    ledcWrite(PWM_CHANNEL, speed);
}

void Motor::SetSpeed(uint8 speed)
{
    this->speed = speed;
    max_speed = speed;
    ledcWrite(PWM_CHANNEL, speed);
}

void Motor::SetSpeedByPercent(float percent)
{
    if (percent > 100.0f || percent < 0.0f) 
    {
#ifdef __DEBUG__
        Serial.println("Motor::SetSpeedByPercent | Percent not within 0-100");
#endif
        return;
    }
    speed = uint8(percent / 100.0f * 255.0f);
    max_speed = speed;
}

void Motor::SetMaxSpeedByPercent(float percent)
{
    if (percent > 100.0f || percent < 0.0f)
    {
#ifdef __DEBUG__
        Serial.println("Motor::SetMaxSpeedByPercent | Percent not within 0-100");
#endif
        return;
    }
    max_speed = percent / 100.0f * 255.0f;
}