#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include <BLECharacteristic.h>
#include <stdint.h>

typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

inline float KMHToMPS(float KMH)
{
    return KMH / 3.6f;
}

inline float InchesToMeters(float in)
{
    return in * 0.0254f;
}

inline float MilimetersToMeters(float mm)
{
    return mm / 1000.0f;
}

inline float RPMToAngularVelocity(uint RPM)
{
    return RPM * 2 * PI / 60.0f;
}

inline uint AngularVelocityToRPM(float angular_velocity)
{
    return (uint)(angular_velocity * 60.0f / (2 * PI));
}

#endif