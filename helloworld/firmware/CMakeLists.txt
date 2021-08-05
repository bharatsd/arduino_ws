cmake_minimum_required(VERSION 2.8.3)
add_compile_options(-std=c++14)


include_directories(${ROS_LIB_DIR})
# include_directories()
include_directories(/home/compositecell2/Arduino/libraries/AccelStepper-/src)
set(RF24_RECURSE True)

# Remove this if using an Arduino without native USB (eg, other than Leonardo)
# add_definitions(-DUSB_CON)

generate_arduino_firmware(hello
  SRCS flipper_2.cpp ${ROS_LIB_DIR}/time.cpp /home/composite-cell2/snap/arduino/56/Arduino/libraries/AccelStepper/src/AccelStepper.cpp 
  HDRS /home/composite-cell2/snap/arduino/56/Arduino/libraries/AccelStepper/src/AccelStepper.h 
  BOARD uno
  PORT /dev/ttyACM0
)
