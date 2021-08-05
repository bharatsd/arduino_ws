#ifndef _ROS_SERVICE_reset_h
#define _ROS_SERVICE_reset_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace visp_hand2eye_calibration
{

static const char RESET[] = "visp_hand2eye_calibration/reset";

  class resetRequest : public ros::Msg
  {
    public:

    resetRequest()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
     return offset;
    }

    virtual const char * getType() override { return RESET; };
    virtual const char * getMD5() override { return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

  class resetResponse : public ros::Msg
  {
    public:

    resetResponse()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
     return offset;
    }

    virtual const char * getType() override { return RESET; };
    virtual const char * getMD5() override { return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

  class reset {
    public:
    typedef resetRequest Request;
    typedef resetResponse Response;
  };

}
#endif
