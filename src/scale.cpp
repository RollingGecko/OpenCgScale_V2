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
    //Serial.println("getWeight Call");
    return weight;
}

int scaleDummy::calibrate(int weight)
{
    setScaleMultiplier(weight / 2);
    Serial.print("calibrate call for Scale element ");
    Serial.println(scaleElementName);
    Serial.print("Calibration Weight: ");
    Serial.println(weight);
    //returns the multiplier
    return getScaleMultiplier();
}

int scaleDummy::getScaleMultiplier()
{
    return scaleMultiplier;
}

void scaleDummy::setScaleMultiplier(int multiplier)
{
    scaleMultiplier = multiplier;
    String message = "ScaleMultiplier in " + String(scaleElementName) + " set to: " + scaleMultiplier;
    Serial.println(message);
};

void scaleDummy::tare()
{
    Serial.print("Tare scale Elenment ");
    Serial.println(scaleElementName);
}

void scaleDummy::setDummyWeight(int dummyWeight)
{
    weight = dummyWeight;
    //String message = "Set weight of " + String(scaleElementName) + " to:" + weight;
    //Serial.println(message);
};

scale::scale(char *elementName){
    scaleElementName = elementName;
};

scale::~scale(){

};
int scale::getWeight(){

};
int scale::getScaleMultiplier(){

};
void scale::setScaleMultiplier(int multiplier){

};
int scale::calibrate(int weight){

};

void scale::setDummyWeight(int dummyWeight){
    
};

void scale::tare(){

};