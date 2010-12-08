#include "boxm2_data_base.h"

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_data_base* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr const& sptr) {}

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base &scene){}
void vsl_b_read(vsl_b_istream& is, boxm2_data_base* p){}
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr const& sptr){}
