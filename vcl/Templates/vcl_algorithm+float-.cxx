#include <vcl_algorithm.txx>

VCL_SWAP_INSTANTIATE(float);

#ifdef GNU_LIBSTDCXX_V3
# include <vcl_vector.h>
VCL_SORT_INSTANTIATE(vcl_vector<float>::iterator, float);
VCL_SORT_INSTANTIATE(vcl_vector<float*>::iterator, float*);
namespace std {
  template float* std::fill_n<float*, unsigned long, float>(float*, unsigned long, float const&);
}
#endif

VCL_SORT_INSTANTIATE(float*, float);
VCL_SORT_INSTANTIATE(float**, float*);
