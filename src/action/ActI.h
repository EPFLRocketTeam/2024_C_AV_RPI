//
// Created by marin on 13.11.2024.
//

#ifndef FLIGHT_COMPUTER_ACTI_H
#define FLIGHT_COMPUTER_ACTI_H

class ActI {
public:
    ActI();
    virtual ~ActI();

    virtual void calibrate();
    virtual bool update();
    virtual void write();
    virtual bool execute();

};

#endif //FLIGHT_COMPUTER_ACTI_H


