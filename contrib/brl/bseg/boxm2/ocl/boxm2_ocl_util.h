#ifndef boxm2_ocl_util_h
#define boxm2_ocl_util_h
//:
// \file
#include <vcl_iostream.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <bocl/bocl_cl.h>
#include <vil/vil_image_view_base.h>
#include <vbl/vbl_array_1d.h>
//: Utility class with static methods
class boxm2_ocl_util
{
  public:
    //: fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
    static void set_persp_camera(vpgl_camera_double_sptr& cam, cl_float* persp_cam);
    static void set_persp_camera(vpgl_perspective_camera<double> * pcam, cl_float* persp_cam);
    static void set_generic_camera(vpgl_camera_double_sptr& cam, cl_float* ray_origins, cl_float* ray_directions); 

    //: fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
    static void set_bit_lookup(cl_uchar* lookup);
    //: private helper method prepares an input image to be processed by update
    static vil_image_view_base_sptr prepare_input_image(vcl_string filename);
    static vil_image_view_base_sptr prepare_input_image(vil_image_view_base_sptr loaded_image);

    static void load_perspective_camera(vcl_string filename, vpgl_perspective_camera<double> & pcam);

    //: this function obtains a cdf of the intensity distribution over a sequcne of image which can be used for rendering images.
    static void get_render_transfer_function(vcl_vector<vcl_string> imgfilenames, float & min, float & max, vbl_array_1d<float> & tf);

 };

#endif // boxm2_ocl_util_h
