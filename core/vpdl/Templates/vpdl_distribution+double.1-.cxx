// Instantiation of vpdl_distribution<double,1>
#include <vpdl/vpdl_distribution.txx>
VPDL_DISTRIBUTION_INSTANTIATE(double,1);
// also instantiate CDF inversion since n==1
VPDL_INVERSE_CDF_INSTANTIATE(double);
