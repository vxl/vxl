#ifndef boxm_opt_opencl_utils_h_
#define boxm_opt_opencl_utils_h_

#include <bcl/bcl_cl.h>
#include <vcl_string.h>
#include <vcl_cstddef.h>
vcl_size_t RoundUp(int global_size,int group_size);
vcl_string error_to_string(cl_int  status );


#endif