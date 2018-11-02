// This is brl/bbas/bsta/io/bsta_io_parzen_sphere.h
#ifndef bsta_io_parzen_sphere_h_
#define bsta_io_parzen_sphere_h_

//:
// \file
// \brief Binary I/O for spherical parzen windows
// \author Joseph L. Mundy
// \date October 12, 2008
//
// \verbatim
//  Modifications
// \endverbatim
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//
#include <iostream>
#include <bsta/bsta_parzen_sphere.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_vector_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save bsta_parzen_sphere to stream.
template <class T, unsigned n>
void
vsl_b_write(vsl_b_ostream &os, const bsta_parzen_sphere<T,n>& g)
{
  vsl_b_write(os,g.bandwidth());

  typedef typename bsta_parzen<T,n>::vector_type vector_;

  std::vector<vector_> samples = g.samples();

  vsl_b_write(os,samples);
}

//: Binary load bsta_parzen_sphere from stream.
template <class T, unsigned n>
void
vsl_b_read(vsl_b_istream &is, bsta_parzen_sphere<T,n>& g)
{
  typedef typename bsta_parzen<T,n>::vector_type vector_;
  std::vector<vector_> samples;
  T bandwidth;
  vsl_b_read(is, bandwidth);
  vsl_b_read(is, samples);
  g.set_bandwidth(bandwidth);
  g.insert_samples(samples);
}

//: Print summary
template <class T, unsigned n>
void
vsl_print_summary(std::ostream &os, const bsta_parzen_sphere<T,n>& g)
{
  os << "parzen (sphere) mean:"<<g.mean()<<" n_samples"<<g.size();
}


#endif // bsta_io_parzen_sphere_h_
