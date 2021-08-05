#include <ros.h>
// #include <iostream>
// #include <string>
#include <std_msgs/Float32.h>
#include <Arduino.h>
#include <ArduinoHardware.h>
// #include <cmath>
//#include "/home/composite-cell2/snap/arduino/56/Arduino/libraries/AccelStepper/src/AccelStepper.h"
#include<AccelStepper.h>
using namespace std;

// namespace std
// {
//   ohserialstream cout(Serial);
// }

////////////////////////////////////////////////////ASH
// // #include SPI library
 #include <SPI.h>
//#include "/snap/arduino/56/libraries/WiFi/extras/wifi_dnld/src/SOFTWARE_FRAMEWORK/DRIVERS/SPI/spi.h"

const int CS = 10;
uint8_t data;               //this will hold our returned data from the AMT20
//uint8_t timeoutcntr;     //our timeout incrementer
//uint16_t currentEncoderPosition;   //this 16 bit variable will hold our 12-bit position



//this is the serial baud rate for talking to the Arduino
#define baudRate 115200

//this will be our SPI timout limit
#define timoutLimit 100

//SPI commands used by the AMT20
#define nop 0x00            //no operation
#define rd_pos 0x10         //read position
#define set_zero_point 0x70 //set zero point

uint8_t data1;               //this will hold our returned data from the AMT20
uint8_t timeoutcntr;     //our timeout incrementer
uint16_t currentPosition;   //this 16 bit variable will hold our 12-bit position
float current_encoder_position;



///////////////////////////////////////////////////ASH END

#define dirPin 3
#define stepPin 4
ros::NodeHandle nh;

std_msgs::Float32 position_motor;
float subscribed_position;
float pos;
std_msgs::Float32 published_actual_pos;
int i = 1;
int joint_status;

float pulse_per_rev = 400*46.656;
float maxsp = pulse_per_rev/20; //20
float maxacc= pulse_per_rev/100; //100
int no_of_steps_per_rev = 200;
ros::Publisher chatter("chatter", &position_motor);
ros::Publisher chatter2("chatter2", &published_actual_pos);
AccelStepper motor_1(1, 3, 4);




//We will use this function to handle transmitting SPI commands in order to keep our code clear and concise.
//It will return the byte received from SPI.transfer()
uint8_t SPIWrite(uint8_t sendByte)
{
  //holder for the received over SPI
//  uint8_t data;

  //the AMT20 requires the release of the CS line after each byte
  digitalWrite(CS, LOW);
  data1 = SPI.transfer(sendByte);
  digitalWrite(CS, HIGH);

  //we will delay here to prevent the AMT20 from having to prioritize SPI over obtaining our position
  delayMicroseconds(10);
  
  return data1;
}

uint16_t encoder(){
// float encoder(){
    //reset the timoutCounter;
    timeoutcntr = 0;
    
    //send the rd_pos command to have the AMT20 begin obtaining the current position
    data = SPIWrite(rd_pos);
  
    //we need to send nop commands while the encoder processes the current position. We
    //will keep sending them until the AMT20 echos the rd_pos command, or our timeout is reached.
    while (data != rd_pos && timeoutcntr++ < timoutLimit)
    {
      data = SPIWrite(nop);
    }
  
  
    if (timeoutcntr < timoutLimit) //rd_pos echo received
    {
      //We received the rd_pos echo which means the next two bytes are the current encoder position.
      //Since the AMT20 is a 12 bit encoder we will throw away the upper 4 bits by masking.
  
      //Obtain the upper position byte. Mask it since we only need it's lower 4 bits, and then
      //shift it left 8 bits to make room for the lower byte.
      currentPosition = (SPIWrite(nop)& 0x0F) << 8;
  
      //OR the next byte with the current position
      currentPosition |= SPIWrite(nop);
    }
    else //timeout reached
    {
        position_motor.data = 9999;
        chatter.publish(&position_motor);
      //This means we had a problem with the encoder, most likely a lost connection. For our
      //purposes we will alert the user via the serial connection, and then stay here forever.
  
      // Serial.write("Error obtaining position.\n");
      // Serial.write("Reset Arduino to restart program.\n");
      
      // while(true);
    }
    currentPosition=currentPosition*0.08789062;
    // Serial.write("Current position: ");
    // Serial.print(currentPosition, DEC); //current position in decimal
    // Serial.write("\n");
    
    
    //Since we are displaying our position over the serial monitor we don't need updates that fast
    // delay(250);
    return currentPosition;
  }


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
    motor_1.moveTo(subscribed_position * pulse_per_rev /360); //input move distance
    nh.spinOnce();
    delay(50);
    while (motor_1.distanceToGo() != 0)
    {
        
        for(int i=0;i<1000;i++){
            motor_1.run();

        }
        current_encoder_position= encoder();
        nh.spinOnce();
//         position_motor.data = (motor_1.currentPosition()) * 360 / pulse_per_rev;
        position_motor.data = current_encoder_position;
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

//     ////////////////////////////////////////////////////ASH
    
// //Initialize the UART serial connection
  // Serial.begin(baudRate);

  //Set I/O mode of all SPI pins.
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(CS, OUTPUT);

  //Initialize SPI using the SPISettings(speedMaxium, dataOrder, dataAMode) function
  //For our settings we will use a clock rate of 500kHz, and the standard SPI settings
  //of MSB First and SPI Mode 0
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  
  //Using SPI.beginTransaction seems to require explicitly setting the beginning state
  //of the CS pin as opposed to the SPI.begin() function that does this for us.
  digitalWrite(CS, HIGH);
//     ////////////////////////////////////////////////////ASH END

    motor_1.setMaxSpeed(maxsp);
    motor_1.setAcceleration(maxacc);
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
    // encoder()

}
