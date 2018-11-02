// This is brl/bbas/bsta/io/bsta_io_mixture_fixed.h
#ifndef bsta_io_mixture_fixed_h_
#define bsta_io_mixture_fixed_h_
//:
// \file
// \brief Binary I/O for a fixed mixture of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 18, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bsta/bsta_mixture_fixed.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save bsta_mixture_fixed to stream.
template <class comp_, unsigned s>
void
vsl_b_write(vsl_b_ostream &os, const bsta_mixture_fixed<comp_,s>& m)
{
  unsigned size = m.num_components();
  vsl_b_write(os,size);
  for (unsigned i=0; i<size; ++i){
    vsl_b_write(os,m.weight(i));
    vsl_b_write(os,m.distribution(i));
  }
}

//: Binary load bsta_mixture_fixed from stream.
template <class comp_, unsigned s>
void
vsl_b_read(vsl_b_istream &is, bsta_mixture_fixed<comp_,s>& m)
{
  while (m.num_components()>0)
    m.remove_last();

  unsigned size;
  vsl_b_read(is,size);
  typename comp_::math_type weight;
  comp_ dstrb;
  for (unsigned i=0; i<size; ++i){
    vsl_b_read(is,weight);
    vsl_b_read(is,dstrb);
    m.insert(dstrb,weight);
  }
}

//: Print summary
template <class comp_, unsigned s>
void
vsl_print_summary(std::ostream &os, const bsta_mixture_fixed<comp_,s>& m)
{
  unsigned size = m.num_components();
  os << "mixture with "<<size<<" components\n";
  for (unsigned i=0; i<size; ++i){
    os<<"  weight:"<<m.weight(i)<<' ';
    vsl_print_summary(os,m.distribution(i));
    os <<"\n";
  }
}


#endif // bsta_io_mixture_fixed_h_
