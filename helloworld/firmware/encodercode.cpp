
//include SPI library
// #include <spi.h>
#include "/snap/arduino/56/libraries/WiFi/extras/wifi_dnld/src/SOFTWARE_FRAMEWORK/DRIVERS/SPI/spi.h"

//this is the serial baud rate for talking to the Arduino
#define baudRate 115200

//this will be our SPI timout limit
#define timoutLimit 100

//SPI commands used by the AMT20
#define nop 0x00            //no operation
#define rd_pos 0x10         //read position
#define set_zero_point 0x70 //set zero point

//set the chip select pin for the AMT20
const int CS = 10;

//Arduino uses a setup function for all program initializations
void setup() 
{
  //Initialize the UART serial connection
  Serial.begin(baudRate);

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
  
}

//After the setup() method this loop gets entered and is the main() function for our program
void loop() 
{
  uint8_t data;               //this will hold our returned data from the AMT20
  uint8_t timeoutCounter;     //our timeout incrementer
  uint16_t currentPosition;   //this 16 bit variable will hold our 12-bit position

  while(true)
  {
    //reset the timoutCounter;
    timeoutCounter = 0;
    
    //send the rd_pos command to have the AMT20 begin obtaining the current position
    data = SPIWrite(rd_pos);
  
    //we need to send nop commands while the encoder processes the current position. We
    //will keep sending them until the AMT20 echos the rd_pos command, or our timeout is reached.
    while (data != rd_pos && timeoutCounter++ < timoutLimit)
    {
      data = SPIWrite(nop);
    }
  
  
    if (timeoutCounter < timoutLimit) //rd_pos echo received
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
      //This means we had a problem with the encoder, most likely a lost connection. For our
      //purposes we will alert the user via the serial connection, and then stay here forever.
  
      Serial.write("Error obtaining position.\n");
      Serial.write("Reset Arduino to restart program.\n");
      
      while(true);
    }
currentPosition=currentPosition*0.08789062;
    Serial.write("Current position: ");
    Serial.print(currentPosition, DEC); //current position in decimal
    Serial.write("\n");
    
    
    //Since we are displaying our position over the serial monitor we don't need updates that fast
    delay(250);
  }
}

//We will use this function to handle transmitting SPI commands in order to keep our code clear and concise.
//It will return the byte received from SPI.transfer()
uint8_t SPIWrite(uint8_t sendByte)
{
  //holder for the received over SPI
  uint8_t data;

  //the AMT20 requires the release of the CS line after each byte
  digitalWrite(CS, LOW);
  data = SPI.transfer(sendByte);
  digitalWrite(CS, HIGH);

  //we will delay here to prevent the AMT20 from having to prioritize SPI over obtaining our position
  delayMicroseconds(10);
  
  return data;
}