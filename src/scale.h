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
};

class scaleDummy : public scaleInterface
{
    //private:

    public:
        scaleDummy();
        ~scaleDummy();
        int getWeight();
        void calibrate();
};


#endif // SCALE_H
