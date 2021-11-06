#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>

class scaleInterface
{
    public:
        scaleInterface(){}
        virtual ~scaleInterface(){}
        virtual int getWeight() = 0;
        virtual void calibrate() = 0;
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
        void calibrate();
        void tare();
};


#endif // SCALE_H
