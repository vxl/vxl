#include <vcl_compiler.h>
//long long - target type will have width of at least 64 bits. (since C++11)
// Disable warning
#ifdef VCL_VC
// 4146: unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4146)
#endif //VCL_VC
#include <vnl/vnl_c_vector.hxx>
VNL_C_VECTOR_INSTANTIATE_ordered(unsigned long long);
