#ifndef boxm2_util_h
#define boxm2_util_h
//:
// \file
#include <vcl_iostream.h>
#include <bocl/bocl_cl.h>
#include <vpgl/vpgl_proj_camera.h>

//: Utility class with static methods
class boxm2_util
{
  public:
    //: fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
    static void set_persp_camera(vpgl_camera_double_sptr cam, cl_float* persp_cam);
    //: fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
    static void set_bit_lookup(cl_uchar* lookup);
};

#endif // boxm2_util_h
