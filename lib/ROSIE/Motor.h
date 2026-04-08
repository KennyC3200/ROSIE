#ifndef MOTOR_H
#define MOTOR_H

#include "Utilities.h"

struct MotorParams
{
    // 1 pin for PWM, and 2 pins for enabling the left and right rotation of the motor
    uint8 PWM_CHANNEL, PWM;
    uint8 L_EN, R_EN;
};

class Motor
{
public:
    Motor();
    Motor(MotorParams params);
    ~Motor();

    void Update();

    // Set the motor speed without accelerating. Also sets the max_speed to be this value. 
    // Large changes in speed can create jerk due to instantaneous change in torque
    uint8 GetSpeed() { return speed; }
    float GetSpeedPercent() { return speed / 255.0f * 100; }
    void SetSpeed(uint8 speed);
    void SetSpeedByPercent(float percent);

    /* Various methods of setting the max speed:
       - Regular max speed 0 to 255 inclusive
       - Set as a percentage of the max speed of the motor
    */
    uint8 GetMaxSpeed() { return max_speed; }
    float GetMaxSpeedPercent() { return float(speed) / max_speed * 100.0f; }
    void SetMaxSpeed(uint8 max_speed) { this->max_speed = max_speed; }
    void SetMaxSpeedByPercent(float percent);
private:
    // Pins for PWM signals and PWM channel
    uint8 PWM_CHANNEL, PWM;

    // Right enable and left enable for the motor module
    uint8 L_EN, R_EN;

    // Current motor speed and the max speed to spin the motor at
    uint8 speed, max_speed;
};

#endif