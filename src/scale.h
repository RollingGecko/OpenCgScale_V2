#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>

class scaleInterface
{
    public:
        scaleInterface(){}
        virtual ~scaleInterface(){}
        virtual int getWeight() = 0;
        virtual int calibrate(int weight) = 0;
        virtual void tare() = 0;
};

class scaleDummy : public scaleInterface
{
    private:
        char *scaleElementName;

    public:
        scaleDummy(char *elementName);
        ~scaleDummy();
        int getWeight();
        int calibrate(int weight);
        void tare();
};


#endif // SCALE_H
