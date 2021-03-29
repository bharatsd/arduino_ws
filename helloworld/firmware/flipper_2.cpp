#include <ros.h>
// #include <iostream>
// #include <string>
#include <std_msgs/Float32.h>
#include <Arduino.h>
#include <ArduinoHardware.h>
// #include <cmath>
#include "/home/bharat/Arduino/libraries/AccelStepper-1.61.0/src/AccelStepper.h"

using namespace std;

// namespace std
// {
//   ohserialstream cout(Serial);
// }

#define dirPin 3
#define stepPin 4

ros::NodeHandle nh;

std_msgs::Float32 position_motor;
float subscribed_position;
float pos;
std_msgs::Float32 published_actual_pos;
int i = 1;
int joint_status;
int pulse_per_rev = 3200;
int no_of_steps_per_rev = 200;
ros::Publisher chatter("chatter", &position_motor);
ros::Publisher chatter2("chatter2", &published_actual_pos);
AccelStepper motor_1(1, 3, 4);

float actualPosition(float &input_angle)
{
    pos += input_angle;
    if (pos >= 360)
    {
        pos = pos - 360;
        return pos;
    }
    else
    {
        return pos;
    }
}

void positionCb(const std_msgs::Float32 &input_position)
{
    // joint_status = 1;

    subscribed_position = (input_position.data);

    published_actual_pos.data = actualPosition(subscribed_position);
    chatter2.publish(&published_actual_pos);
    nh.spinOnce();
    motor_1.moveTo(subscribed_position * pulse_per_rev / 360); //input move distance
    nh.spinOnce();
    delay(50);
    while (motor_1.distanceToGo() != 0)
    {
        nh.spinOnce();
        motor_1.run();
        position_motor.data = (motor_1.currentPosition()) * 360 / pulse_per_rev;
        chatter.publish(&position_motor);
    }
    nh.spinOnce();
    // joint_status = 0;
    // chatter.publish(&position_motor);
    // joint_status = 0;
}

ros::Subscriber<std_msgs::Float32> sub("arduino_position_topic", &positionCb);

void setup()
{
    // put your setup code here, to run once:
    nh.initNode();

    motor_1.setMaxSpeed(2500);
    motor_1.setAcceleration(100);
    // motor_1.setSpeed(45);
    motor_1.setEnablePin();
    nh.advertise(chatter);
    nh.advertise(chatter2);
    nh.subscribe(sub);

    
}

void loop()
{
    nh.spinOnce();
    delay(1);
}