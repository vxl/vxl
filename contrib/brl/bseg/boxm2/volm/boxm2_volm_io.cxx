//:
// \file
#include <iostream>
#include "boxm2_volm_io.h"
#include "boxm2_volm_locations.h";
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save to stream
void vsl_b_write(vsl_b_ostream &os, const boxm2_volm_loc_hypotheses& m)
{
  m.b_write(os);
}

//: Binary load from stream.
void vsl_b_read(vsl_b_istream &is, boxm2_volm_loc_hypotheses& m)
{
  m.b_read(is);
}

//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_volm_loc_hypotheses* &p){p->b_write(os);}
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_volm_loc_hypotheses_sptr& sptr){sptr->b_write(os);}
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_volm_loc_hypotheses_sptr const& sptr){sptr->b_write(os);}

//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses* p){p->b_read(is);}
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses_sptr& sptr){sptr->b_read(is);}
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses_sptr const& sptr){std::cerr<<"vsl_b_read not implemented for boxm2_volm_loc_hypotheses_sptr const!!";}
