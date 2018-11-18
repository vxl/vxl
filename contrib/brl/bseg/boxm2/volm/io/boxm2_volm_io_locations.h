#ifndef boxm2_volm_io_locations_h_
#define boxm2_volm_io_locations_h_
//:
// \file

#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/volm/boxm2_volm_locations.h>


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, boxm2_volm_loc_hypotheses const &loc_hyp);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, boxm2_volm_loc_hypotheses &loc_hyp);

void vsl_print_summary(std::ostream &os, const boxm2_volm_loc_hypotheses &loc_hyp);

void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses* p);

void vsl_b_write(vsl_b_ostream& os, const boxm2_volm_loc_hypotheses* &p);

void vsl_print_summary(std::ostream& os, const boxm2_volm_loc_hypotheses* &p);

#endif
