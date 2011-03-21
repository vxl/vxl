#ifndef boxm2_view_utils_h
#define boxm2_view_utils_h
//: \brief boxm2_view_utils contains factored out methods used in multiple tableaus
// \file
#include <vcl_iostream.h>
#include <bocl/bocl_cl.h>
#include "boxm2_include_glew.h"

//: Utility class with static methods
class boxm2_view_utils
{
  public:

    //: creates a CL_GL context on the input device
    static cl_context create_clgl_context(cl_device_id& device);

 };

#endif // boxm2_view_utils_h
