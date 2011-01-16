// This is gel/mrc/vpgl/icam/icam_ocl/icam_ocl_utils.h
#ifndef icam_ocl_utils_h
#define icam_ocl_utils_h
//:
// \file
// \brief  Util functions
// \author Vishal Jain vj@lems.brown.edu
// \date  May 30, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include "icam_ocl/icam_ocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>

#define SDK_SUCCESS 0
#define SDK_FAILURE 1
//#define GROUP_SIZE 64
#define VECTOR_SIZE 4

//: function to pad the global threads if it is not exact multiple of the workgroup size
vcl_size_t RoundUp(int global_size,int group_size);
//: function to display decoded error message.
vcl_string error_to_string(cl_int  status );
//: Check for error returns
int check_val(cl_int status, cl_int result, std::string message);

#endif
