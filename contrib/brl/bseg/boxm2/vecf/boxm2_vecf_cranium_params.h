#ifndef boxm2_vecf_cranium_params_h_
#define boxm2_vecf_cranium_params_h_
// :
// \file
// \brief  Parameters for the cranium model
//
// \author J.L. Mundy
// \date   3 November 2015
//
#include "boxm2_vecf_articulated_params.h"
#include <vcl_iostream.h>
#include <vgl/vgl_vector_3d.h>
class boxm2_vecf_cranium_params : public boxm2_vecf_articulated_params
{
public:

  boxm2_vecf_cranium_params() :  cranium_intensity_(static_cast<unsigned char>(175) ),
    offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0) )
  {
  }

  boxm2_vecf_cranium_params(unsigned char cranium_intensity) : cranium_intensity_(cranium_intensity)
  {
  }

  unsigned char         cranium_intensity_;
  vgl_vector_3d<double> offset_;
};
vcl_ostream & operator <<(vcl_ostream& s, boxm2_vecf_cranium_params const& pr);

vcl_istream & operator >>(vcl_istream& s, boxm2_vecf_cranium_params& pr);

#endif// boxm2_vecf_cranium_params
