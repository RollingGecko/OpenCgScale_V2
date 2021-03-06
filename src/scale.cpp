#include <Arduino.h>
#include "scale.h"
#include <HX711.h>

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

float scaleDummy::calibrate(int weight)
{
    setScaleMultiplier(weight / 2);
    Serial.print("calibrate call for Scale element ");
    Serial.println(scaleElementName);
    Serial.print("Calibration Weight: ");
    Serial.println(weight);
    //returns the multiplier
    return getScaleMultiplier();
}

float scaleDummy::getScaleMultiplier()
{
    return scaleMultiplier;
}

void scaleDummy::setScaleMultiplier(float multiplier)
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

scale::~scale(){};

void scale::init(byte dataPin, byte clkPin, byte gain){
    hx711Module.begin(dataPin, clkPin, gain);
};
int scale::getWeight(){
    int scaleReading = 0;
	hx711Module.read();
	scaleReading = round(hx711Module.get_units(5));
	if (scaleReading <= 4)
	{
		scaleReading = 0;
	}
    return scaleReading;
};
float scale::getScaleMultiplier(){
    return hx711Module.get_scale();
};
void scale::setScaleMultiplier(float multiplier){
    hx711Module.set_scale(multiplier);
};

float scale::calibrate(int weight)
{
    float newMultiplier = 0;
    Serial.println("Calibration " + String(scaleElementName) + " startet");

    float reading = hx711Module.get_units(10);
    newMultiplier = (reading / weight) * hx711Module.get_scale();
    hx711Module.set_scale(newMultiplier);
    return hx711Module.get_scale();
};

void scale::tare(){
    hx711Module.tare();
};