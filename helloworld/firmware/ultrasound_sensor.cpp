#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <Arduino.h>
#include <ArduinoHardware.h>

using namespace std;

ros::NodeHandle nh;

const int pingPin = 12; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 11; // Echo Pin of Ultrasonic Sensor

std_msgs::Float32 sheet_position;
std_msgs::String bound;
ros::Publisher sheet_pos("sheet_state_on_tool",&bound);

float microsecondsToInches(float microseconds) {
   return microseconds / 74 / 2;
}

float microsecondsToCentimeters(float microseconds) {
   return microseconds / 29 / 2;
}

void setup() {
  nh.initNode();
  nh.advertise(sheet_pos);
//   Serial.begin(9600); // Starting Serial Terminal
}

void loop() {
   float duration, inches, cm;
   std_msgs::String temp;
   nh.spinOnce();
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   nh.spinOnce();
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   sheet_position.data = cm;
   if(cm>=10 && cm<=15){
       temp.data = "In bounds";
       sheet_pos.publish(&temp);
   }
   else
   {
       temp.data = "Drooping";
       sheet_pos.publish(&temp);
   }
//    sheet_pos.publish(&sheet_position);
   nh.spinOnce();
//   Serial.print(inches);
//   Serial.print("in, ");
//   Serial.print(cm);
//   Serial.print("cm");
//   Serial.println();
   delay(100);
}

