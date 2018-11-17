#ifndef bstm_ocl_util_h
#define bstm_ocl_util_h
//:
// \file
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_cl.h>
#include <vbl/vbl_array_1d.h>

//: Utility class with static methods
class bstm_ocl_util
{
  public:

    //: fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
    static void set_ocl_camera(vpgl_camera_double_sptr& cam, cl_float* persp_cam);
    static void set_persp_camera(vpgl_perspective_camera<double> * pcam, cl_float* persp_cam);
    static void set_proj_camera(vpgl_proj_camera<double> * pcam, cl_float* cam);
    static void set_generic_camera(vpgl_camera_double_sptr& cam, cl_float* ray_origins, cl_float* ray_directions, unsigned cl_ni, unsigned cl_nj);

    //: fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
    static void set_bit_lookup(cl_uchar* lookup);

    static void load_perspective_camera(const std::string& filename, vpgl_perspective_camera<double> & pcam);

    //: this function obtains a cdf of the intensity distribution over a sequcne of image which can be used for rendering images.
    static void get_render_transfer_function(std::vector<std::string> imgfilenames, float & min, float & max, vbl_array_1d<float> & tf);

    //: mog optoins
    static std::string mog_options(const std::string& data_type);

    //: label optoins
    static std::string label_options(const std::string& data_type);
 };

#endif // bstm_ocl_util_h
