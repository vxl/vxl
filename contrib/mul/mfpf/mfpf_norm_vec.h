#ifndef mfpf_norm_vec_h_
#define mfpf_norm_vec_h_
//:
// \file
// \brief Sets vec to have zero mean and unit length
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Sets vec to have zero mean and unit length
inline void mfpf_norm_vec(vnl_vector<double>& vec, double var_min=1.0E-6,
                          double* pvar=nullptr)
{
  unsigned n=vec.size();
  double *v=vec.data_block();
  double sum=0.0,sum_sq=0.0;
  for (unsigned i=0;i<n;++i)
  {
    sum+=v[i]; sum_sq+=v[i]*v[i];
  }
  double mean = sum/n;
  double ss = std::max(var_min,sum_sq-n*mean*mean);
  double s = std::sqrt(ss);

  for (unsigned i=0;i<n;++i) v[i]=(v[i]-mean)/s;
  if(pvar) //optionally return variance
      *pvar=ss;
}

#endif // mfpf_norm_vec_h_
