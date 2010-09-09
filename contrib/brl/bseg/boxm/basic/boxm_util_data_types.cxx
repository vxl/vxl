#include <boxm/basic/boxm_util_data_types.h>

// Binary write boxm_render_probe_manager scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_array_1d_float const& a){}

// Binary load boxm_render_probe_manager scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_array_1d_float &a){}

void vsl_b_read(vsl_b_istream& is, boxm_array_1d_float* b){}

// Binary write  scene pointer to stream
void vsl_b_write(vsl_b_ostream& os, const boxm_array_1d_float* &b){}
