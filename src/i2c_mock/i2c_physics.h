
#ifndef I2C_PHYSICS_H
#define I2C_PHYSICS_H

struct Vec3 {
    float x, y, z;
};

struct I2CPhysics {
private:
    static I2CPhysics* physics;
public:
    static I2CPhysics* getInstance () {
        return physics;
    }
    static void setInstance (I2CPhysics* _physics) {
        I2CPhysics::physics = _physics;
    }

    virtual void start () = 0;

    virtual Vec3 position () = 0;
    virtual Vec3 speed () = 0;
    virtual Vec3 acceleration () = 0;

    virtual Vec3 angle () = 0;
    virtual Vec3 angular_speed () = 0;
    virtual Vec3 angular_acceleration () = 0;
};

struct I2CPhysics_NoMovement : public I2CPhysics {
    void start () override {}

    Vec3 position () override { return { 0, 0, 0 }; }
    Vec3 speed () override { return { 0, 0, 0 }; }
    Vec3 acceleration () override { return { 0, 0, 0 }; }

    Vec3 angle () override { return { 0, 0, 0 }; }
    Vec3 angular_speed () override { return { 0, 0, 0 }; }
    Vec3 angular_acceleration () override { return { 0, 0, 0 }; }
};

#endif
