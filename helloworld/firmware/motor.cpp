#include <ros.h>
// #include <string>
#include <std_msgs/Float32.h>
#include <Arduino.h>
#include <ArduinoHardware.h>
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
int i = 1;
int joint_status = 0;
ros::Publisher chatter("chatter", &position_motor);
AccelStepper motor_1(1, 4, 3);

void positionCb(const std_msgs::Float32 &input_position)
{
  joint_status = 1;
  subscribed_position = input_position.data;
  // Serial.print(subscribed_position);
  // cout
}

ros::Subscriber<std_msgs::Float32> sub("arduino_position_topic", &positionCb);

void setup()
{
  // put your setup code here, to run once:
  nh.initNode();

  nh.subscribe(sub);
  nh.advertise(chatter);

  motor_1.setMaxSpeed(2500);
  motor_1.setAcceleration(100);
  // motor_1.setSpeed(45);
  motor_1.setEnablePin();
}

void loop()
{
  // motor_1.run();
  if (joint_status == 1)
  {
    // pos = subscribed_position;
    motor_1.moveTo(subscribed_position); //input move distance
    nh.spinOnce();
    delay(50);
    while (motor_1.distanceToGo() != 0)
    {
      nh.spinOnce();
      // motor_1.
      motor_1.run();
      position_motor.data = motor_1.currentPosition();
      chatter.publish(&position_motor);
      // nh.spinOnce();
      // delay(10);
    }
  }
  // motor_1.run();
  joint_status = 0;

  nh.spinOnce();
  delay(1);

  // motor_1.runSpeed();
  // motor_1.run();
}