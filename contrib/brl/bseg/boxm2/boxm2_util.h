#ifndef boxm2_util_h
#define boxm2_util_h

#include <vcl_iostream.h>
#include <bocl/bocl_cl.h>
#include <vpgl/vpgl_proj_camera.h>

//: Utility class with static methods
class boxm2_util
{
  public:
    static void set_persp_camera(vpgl_camera_double_sptr cam, cl_float* persp_cam); 
    static void set_bit_lookup(cl_uchar* lookup);
}; 

#endif // boxm2_render_tableau_h
