#include <stepper_ros/stepper.h>

namespace stepper_ros_node
{
    Stepper::Stepper(int stepPin, int dirPin) : stepper(AccelStepper::DRIVER, stepPin, dirPin)
    {
        home_reached = 1;
        pinMode(home_switch_power, OUTPUT);
        digitalWrite(home_switch_power, HIGH);
        pinMode(home_switch, INPUT);
        set_params();
        // microsteps = 8;
        // set_microsteps(microsteps);
        stepper.stop();
    }

    Stepper::~Stepper()
    {
    }

    void Stepper::set_homing_params()
    {
        stepper.setMaxSpeed(50);
        stepper.setAcceleration(20);
    }

    void Stepper::set_params()
    {
        stepper.setMaxSpeed(200);    //default: 100
        stepper.setAcceleration(60); //default: 40
    }

    void Stepper::homing()
    {
        /*This function is blocking because it runs in a while loop, until homing is completed. This is useful in this case.
     Based on: https://www.brainy-bits.com/setting-stepper-motors-home-position-using-accelstepper/
  */
        set_homing_params();
        stepper.setCurrentPosition(0);

        long initial_homing = -1;
        while (digitalRead(home_switch))
        {
            stepper.moveTo(initial_homing);
            initial_homing--;
            stepper.run();
            delay(5);
        }
        stepper.stop();

        stepper.setCurrentPosition(-degToSteps(44));

        set_params();

        homing_completed = 0;
    }

}