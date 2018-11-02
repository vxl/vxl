// This is brl/bbas/bsta/io/bsta_io_von_mises.h
#ifndef bsta_io_von_mises_h_
#define bsta_io_von_mises_h_
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
#include <bsta/bsta_von_mises.h>
#include <bsta/bsta_parzen.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save bsta_von_mises to stream.
template <class T, unsigned n>
void
vsl_b_write(vsl_b_ostream &os, const bsta_von_mises<T,n>& vm)
{
  vsl_b_write(os,vm.kappa());

  vsl_b_write(os,vm.mean());
}

//: Binary load bsta_von_mises from stream.
template <class T, unsigned n>
void
vsl_b_read(vsl_b_istream &is, bsta_von_mises<T,n>& vm)
{
  typename bsta_parzen<T,n>::vector_type mean;
  T kappa;
  vsl_b_read(is, kappa);
  vsl_b_read(is, mean);
  vm.set_kappa(kappa);
  vm.set_mean(mean);
}

//: Print summary
template <class T, unsigned n>
void
vsl_print_summary(std::ostream &os, const bsta_von_mises<T,n>& vm)
{
  os << "von_mises  mean:"<<vm.mean()<<" n_samples"<<vm.size();
}


#endif // bsta_io_von_mises_h_
