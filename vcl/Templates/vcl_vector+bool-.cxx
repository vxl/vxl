#include <vcl/vcl_vector.h>
#include <vcl/vcl_vector.txx>

#if defined(NO_STD_BOOL) || defined(VCL_SUNPRO_CC_50)
// SunPro 5.0 provides vector<bool> as a 
// specialization of vector<T>.
#else
VCL_VECTOR_INSTANTIATE(bool);
#endif
