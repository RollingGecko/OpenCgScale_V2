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

int scaleDummy::calibrate(int weight)
{
    Serial.print("calibrate call for Scale element ");
    Serial.println(scaleElementName);
    Serial.print("Calibration Weight: ");
    Serial.println(weight);
//returns the multiplier
    return 212;

}

String scaleDummy::getScaleMultiplier()
{
    String value = "212";
    return value;
}


void scaleDummy::tare(){
    Serial.print ("Tare scale Elenment ");
    Serial.println(scaleElementName);
}