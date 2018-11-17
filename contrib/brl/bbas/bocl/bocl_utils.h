// This is bocl_utils.h
#ifndef bocl_utils_h
#define bocl_utils_h
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
#include <string>
#include <iostream>
#include <cstddef>
#include "bocl/bocl_cl.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define SDK_SUCCESS 0
#define SDK_FAILURE 1

#define ARRAY_BUFFER_NAME "array_buf"
#define RESULT_ARRAY_BUFFER_NAME "result_array_buf"
#define CL_LEN_BUFFER_NAME "cl_len_buf"
#define RESULT_FLAG_BUFFER_NAME "result_flag_buf"

//: function to pad the global threads if it is not exact multiple of the workgroup size
std::size_t RoundUp(int global_size,int group_size);
//: function to display decoded error message.
std::string error_to_string(cl_int  status );
//: Check for error returns
int check_val(cl_int status, cl_int result, const std::string& message);

#endif
