#ifndef boxm2_vecf_mandible_params_h_
#define boxm2_vecf_mandible_params_h_
//:
// \file
// \brief  Parameters for the mandible model
//
// \author J.L. Mundy
// \date   24 Aug 2015
//
#include "boxm2_vecf_articulated_params.h"
#include <vcl_iostream.h>

class boxm2_vecf_mandible_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_mandible_params():  ramus_length_(1.0){
  }

 boxm2_vecf_mandible_params(double ramus_length):ramus_length_(ramus_length){
  }
  double ramus_length_;
};
vcl_ostream&  operator << (vcl_ostream& s, boxm2_vecf_mandible_params const& pr);
vcl_istream&  operator >> (vcl_istream& s, boxm2_vecf_mandible_params& pr);

#endif// boxm2_vecf_mandible_params
