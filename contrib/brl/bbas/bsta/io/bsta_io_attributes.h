// This is brl/bbas/bsta/io/bsta_io_attributes.h
#ifndef bsta_io_attributes_h_
#define bsta_io_attributes_h_
//:
// \file
// \brief Binary I/O for attributed distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 28, 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bsta/bsta_attributes.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save bsta_attributes to stream.
template <class comp_>
void
vsl_b_write(vsl_b_ostream &os, const bsta_num_obs<comp_>& m)
{
  comp_ n=static_cast<comp_>(m);

  //std::cout<<sizeof(n)<<' '<<sizeof(m)<<':';
  vsl_b_write(os,n);
  //vsl_b_write(os,(int)1);
  vsl_b_write(os,m.num_observations);
}

//: Binary load bsta_attributes from stream.
template <class comp_>
void
vsl_b_read(vsl_b_istream &is, bsta_num_obs<comp_>& m)
{
  comp_ dstrb;
  vsl_b_read(is,dstrb);
  // int a; vsl_b_read(is,a);
  m = bsta_num_obs<comp_>(dstrb);
  vsl_b_read(is,m.num_observations);
}

//: Print summary
template <class comp_>
void
vsl_print_summary(std::ostream &os, const bsta_num_obs<comp_>& m)
{
  vsl_print_summary(os,static_cast<comp_>(m));
  os << " with "<<m.num_observations<<" observations";
}


#endif // bsta_io_attributes_h_
