#include <Arduino.h>
#include "scale.h"


scaleDummy::scaleDummy()
{

}

scaleDummy::~scaleDummy()
{
    
}

int scaleDummy::getWeight()
{
    Serial.println("getWeight Call");
    return 500;
}

void scaleDummy::calibrate()
{
    Serial.println("calibrate call");
}