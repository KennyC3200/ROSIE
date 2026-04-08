#include "ROSIE.h"

ROSIE* rosie = nullptr;

void setup()
{
    rosie = new ROSIE();
}

void loop()
{
    rosie->Loop();
}