#include <vcl_algorithm.txx>

#ifdef GNU_LIBSTDCXX_V3
# include <vcl_vector.h>
VCL_SORT_INSTANTIATE(vcl_vector<double>::iterator, double);
VCL_SORT_INSTANTIATE(vcl_vector<double*>::iterator, double*);
namespace std
{
  template double* std::fill_n<double*, unsigned long, double>(double*, unsigned long, double const&);
}
#endif

VCL_SWAP_INSTANTIATE(double);
VCL_UNIQUE_INSTANTIATE(double *);
VCL_SORT_INSTANTIATE(double*, double);
VCL_SORT_INSTANTIATE(double**, double*);
