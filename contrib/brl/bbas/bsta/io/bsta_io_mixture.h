// This is brcv/seg/bsta/io/bsta_io_mixture.h
#ifndef bsta_io_mixture_h_
#define bsta_io_mixture_h_

//:
// \file
// \brief Binary I/O for a mixture of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_mixture.h>
#include <vsl/vsl_binary_io.h>

//: Binary save bsta_mixture to stream.
template <class _comp>
void
vsl_b_write(vsl_b_ostream &os, const bsta_mixture<_comp>& m)
{
  unsigned size = m.num_components();
  vsl_b_write(os,size);
  for(unsigned i=0; i<size; ++i){
    vsl_b_write(os,m.weight(i));
    vsl_b_write(os,m.distribution(i));
  }
}

//: Binary load bsta_mixture from stream.
template <class _comp>
void
vsl_b_read(vsl_b_istream &is, bsta_mixture<_comp>& m)
{
  while(m.num_components()>0)
    m.remove_last();

  unsigned size;
  vsl_b_read(is,size);
  typename _comp::math_type weight;
  _comp dstrb;
  for(unsigned i=0; i<size; ++i){
    vsl_b_read(is,weight);
    vsl_b_read(is,dstrb);
    m.insert(dstrb,weight);
  }
}

//: Print summary
template <class _comp>
void
vsl_print_summary(vcl_ostream &os, const bsta_mixture<_comp>& m)
{
  unsigned size = m.num_components();
  os << "mixture with "<<size<<" components\n";
  for(unsigned i=0; i<size; ++i){
    os<<"  weight:"<<m.weight(i)<<" ";
    vsl_print_summary(os,m.distribution(i));
    os <<"\n";
  }
}


#endif // bsta_io_mixture_h_
