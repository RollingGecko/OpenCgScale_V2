#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include <HX711.h>
class scaleInterface
{
    public:
        scaleInterface(){}
        virtual ~scaleInterface(){}
        virtual void init(byte dataPin, byte clkPin, byte gain) = 0;
        virtual int getWeight() = 0;
        virtual int calibrate(int weight) = 0;
        virtual float getScaleMultiplier() = 0;
        virtual void setScaleMultiplier(float multiplier) = 0;
        virtual void tare() = 0;
        virtual void setDummyWeight(int dummyWeight) = 0;
};

class scaleDummy : public scaleInterface
{
    private:
        char *scaleElementName;
        int scaleMultiplier;
        int weight = 500;

    public:
        scaleDummy(char *elementName);
        ~scaleDummy();
        void init(byte dataPin, byte clkPin, byte gain){};
        int getWeight();
        float getScaleMultiplier();
        void setScaleMultiplier(float multiplier);
        int calibrate(int weight);
        void setDummyWeight(int dummyWeight);
        void tare();
};

class scale : public scaleInterface
{
    private:
        char *scaleElementName;
        HX711 *hx711Module;

    public:
        scale(char *elementName);
        ~scale();
        void init(byte dataPin, byte clkPin, byte gain);
        int getWeight();
        float getScaleMultiplier();
        void setScaleMultiplier(float multiplier);
        int calibrate(int weight);
        void setDummyWeight(int dummyWeight){}; //will not be used. How to avoid?
        void tare();

};

#endif // SCALE_H
