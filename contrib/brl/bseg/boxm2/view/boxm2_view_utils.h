#ifndef boxm2_view_utils_h
#define boxm2_view_utils_h
//:
// \file
// \brief boxm2_view_utils contains factored out methods used in multiple tableaus
#include <iostream>
#include <vector>
#include <vnl/vnl_math.h>
#include "boxm2_include_glew.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_cl.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_camera.h>

//: Utility class with static methods
class boxm2_view_utils
{
  public:

    //: creates a CL_GL context on the input device
    static cl_context create_clgl_context(cl_device_id& device);

    //: function to convert vpgl camera to vrml orientation
    static void convert_camera_to_vrml(vgl_vector_3d<double>  look, vgl_vector_3d<double> up, vgl_vector_3d<double> & res_axis,double & res_angle);

    //: code to add vrml animation from given viewpoints. This function appends the animation at the end of the stream.
    static void add_vrml_animation(std::ostream & os,std::vector<vpgl_camera_double_sptr > &  cameras);
 };

#endif // boxm2_view_utils_h
