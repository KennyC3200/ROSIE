#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

#include "Utilities.h"
#include "Motor.h"

struct MotorManagerParams
{
    int cnt;
    uint8 speed_ROC_delay;
};

class MotorManager
{
public:
    MotorManager(MotorManagerParams params);
    ~MotorManager();

    void EmergencyShutdown();
    void AppendMotor(MotorParams params);

    void Update();

    // Set the rate of change delay for the motor while accelerating/decelerating
    uint GetSpeedROCDelay() { return speed_ROC_delay; }
    void SetSpeedROCDelay(uint delay) { speed_ROC_delay = speed_ROC_delay; }

    // Get the current speed of a motor
    uint GetSpeed(uint motor);
    void SetSpeed(uint motor, uint8 speed);
    void SetSpeedByPercent(uint motor, float percent);

    uint GetMaxSpeed(uint motor);
    void SetMaxSpeed(uint motor, uint8 max_speed);
    void SetMaxSpeedByPercent(uint motor, float percent);
private:
    Motor* motors;
    uint cnt, curr_cnt;
    uint speed_ROC_delay;
};

#endif