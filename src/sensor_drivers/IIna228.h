#ifndef IINA228_H
#define IINA228_H

class IIna228 {
public:
    virtual ~IIna228() = default;

    virtual float getBusVoltage() = 0;
    virtual int setMaxCurrentShunt(float maxCurrent, float shunt) = 0;
};

#endif

