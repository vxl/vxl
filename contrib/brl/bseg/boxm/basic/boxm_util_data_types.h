#ifndef boxm_util_data_types_h
#define boxm_util_data_types_h
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vsl/vsl_binary_io.h>

class boxm_array_1d_float: public vbl_ref_count
{
 public:
  boxm_array_1d_float(unsigned n):data_array(n,-1.0f){}
  vbl_array_1d<float> data_array;
};

typedef vbl_smart_ptr<boxm_array_1d_float> boxm_array_1d_float_sptr;

//: Binary write boxm_render_probe_manager scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_array_1d_float const& a);

//: Binary load boxm_render_probe_manager scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_array_1d_float &a);

void vsl_b_read(vsl_b_istream& is, boxm_array_1d_float* b);

//: Binary write  scene pointer to stream
void vsl_b_write(vsl_b_ostream& os, const boxm_array_1d_float* &b);

#endif
