#include "boxm2_data_base.h"
//:
// \file

//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base const& scene) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_data_base* &p) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr& sptr) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr const& sptr) {}

//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base &scene){}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base* p){}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr& sptr){}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr const& sptr){}
