#ifndef ITMP1075_H
#define ITMP1075_H

class ITmp1075 {
public:
    virtual ~ITmp1075() = default;

    virtual float getTemperatureCelsius() = 0;
};

#endif
