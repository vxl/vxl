#ifndef mfpf_norm_vec_h_
#define mfpf_norm_vec_h_
//:
// \file
// \brief Sets vec to have zero mean and unit length
// \author Tim Cootes

#include <vnl/vnl_vector.h>
#include <vcl_algorithm.h> // std::max()

//: Sets vec to have zero mean and unit length
inline void mfpf_norm_vec(vnl_vector<double>& vec)
{
  unsigned n=vec.size();
  double *v=vec.data_block();
  double sum=0.0,sum_sq=0.0;
  for (unsigned i=0;i<n;++i)
  {
    sum+=v[i]; sum_sq+=v[i]*v[i];
  }
  double mean = sum/n;
  double ss = vcl_max(1e-6,sum_sq-n*mean*mean);
  double s = vcl_sqrt(ss);

  for (unsigned i=0;i<n;++i) v[i]=(v[i]-mean)/s;
}

#endif // mfpf_norm_vec_h_
