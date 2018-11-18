#include "bsta_random_wrapper.h"
//:
// \file

//--- IO read/write for sptrs--------------------------------------------------
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bsta_random_wrapper const&  /*scene*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, const bsta_random_wrapper* & /*p*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, bsta_random_wrapper_sptr&  /*sptr*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, bsta_random_wrapper_sptr const&  /*sptr*/) {}

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bsta_random_wrapper & /*scene*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bsta_random_wrapper*  /*p*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bsta_random_wrapper_sptr&  /*sptr*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bsta_random_wrapper_sptr const&  /*sptr*/) {}
