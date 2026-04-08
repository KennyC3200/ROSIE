#include "MotorManager.h"
#include <Arduino.h>

MotorManager::MotorManager(MotorManagerParams params)
    : cnt(params.cnt)
    , curr_cnt(0)
    , speed_ROC_delay(params.speed_ROC_delay)
{
    motors = new Motor[cnt];
}

MotorManager::~MotorManager()
{
    delete[] motors;
}

void MotorManager::EmergencyShutdown()
{
    for (uint i = 0; i < curr_cnt; i++)
        motors[i].SetSpeed(0);
}

void MotorManager::AppendMotor(MotorParams params)
{
    if (curr_cnt < cnt)
    {
        motors[curr_cnt] = Motor(params);
        curr_cnt++;
    }
}

void MotorManager::Update()
{
    for (uint i = 0; i < curr_cnt; i++)
    {
        motors[i].Update();
        delay(speed_ROC_delay);
    }
}

uint MotorManager::GetSpeed(uint motor) 
{ 
    if (motor < curr_cnt)
        return motors[motor].GetSpeed(); 
#ifdef __DEBUG__
    Serial.println("MotorManager::GetSpeed | Index out of bounds");
#endif
    return 0;
}

void MotorManager::SetSpeed(uint motor, uint8 speed) 
{ 
    if (motor < curr_cnt)
        motors[motor].SetSpeed(speed); 
#ifdef __DEBUG__
    else
    Serial.println("MotorManager::SetSpeed | Index out of bounds");
#endif
}

void MotorManager::SetSpeedByPercent(uint motor, float percent)
{
    if (motor < curr_cnt)
        motors[motor].SetSpeedByPercent(percent);
#ifdef __DEBUG__
    else
        Serial.println("MotorManager::SetSpeedByPercent | Index out of bounds");
#endif
}

uint MotorManager::GetMaxSpeed(uint motor)
{
    if (motor < curr_cnt)
        return motors[motor].GetMaxSpeed();
#ifdef __DEBUG__
    Serial.println("MotorManager::GetMaxSpeed | Index out of bounds");
#endif
    return 0;
}

void MotorManager::SetMaxSpeed(uint motor, uint8 max_speed)
{
    if (motor < curr_cnt)
        motors[motor].SetMaxSpeed(max_speed);
#ifdef __DEBUG__
    else
    Serial.println("MotorManager::SetMaxSpeed | Index out of bounds");
#endif
}

void MotorManager::SetMaxSpeedByPercent(uint motor, float percent)
{
    if (motor < curr_cnt)
        motors[motor].SetMaxSpeedByPercent(percent);
#ifdef __DEBUG__
    else
        Serial.println("MotorManager::SetMaxSpeedByPercent | Index out of bounds");
#endif
}