// This is brl/bbas/bsta/io/bsta_io_attributes.h
#ifndef bsta_io_attributes_h_
#define bsta_io_attributes_h_
//:
// \file
// \brief Binary I/O for attributed distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_attributes.h>
#include <vsl/vsl_binary_io.h>

//: Binary save bsta_attributes to stream.
template <class comp_>
void
vsl_b_write(vsl_b_ostream &os, const bsta_num_obs<comp_>& m)
{
  vsl_b_write(os,static_cast<comp_>(m));
  vsl_b_write(os,m.num_observations);
}

//: Binary load bsta_attributes from stream.
template <class comp_>
void
vsl_b_read(vsl_b_istream &is, bsta_num_obs<comp_>& m)
{
  comp_ dstrb;
  vsl_b_read(is,dstrb);
  m = bsta_num_obs<comp_>(dstrb);
  vsl_b_read(is,m.num_observations);
}

//: Print summary
template <class comp_>
void
vsl_print_summary(vcl_ostream &os, const bsta_num_obs<comp_>& m)
{
  vsl_print_summary(os,static_cast<comp_>(m));
  os << " with "<<m.num_observations<<" observations";
}


#endif // bsta_io_attributes_h_
