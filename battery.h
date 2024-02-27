#ifndef BATTERY_H
#define BATTERY_H

class Battery {
public:
    Battery() {}
    ~Battery() {}

    void update();

    inline double get_voltage() const { return voltage; }
    inline double get_current() const { return current; }
private:
    double voltage;
    double current;
};

#endif /* BATTERY_H */