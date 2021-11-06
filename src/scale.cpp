#include <Arduino.h>
#include "scale.h"


scaleDummy::scaleDummy(char *elementName)
{
    scaleElementName = elementName;
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

void scaleDummy::tare(){
    Serial.print ("Tare scale Elenment ");
    Serial.println(scaleElementName);
}