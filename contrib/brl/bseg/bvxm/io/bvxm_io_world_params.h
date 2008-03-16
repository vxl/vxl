#ifndef bvxm_io_world_params_h_
#define bvxm_io_world_params_h_
//:
// \file

#include <vsl/vsl_binary_io.h>
#include "../bvxm_world_params.h"

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvxm_world_params const& params);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvxm_world_params &params);

#endif // bvxm_io_world_params_h_
