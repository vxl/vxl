// Disable warning
#include <vcl_compiler.h>
#ifdef VCL_VC
// 4146: unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4146)
#endif //VCL_VC

// Instantiation of vgl_vector_2d<unsigned int>
#include <vgl/vgl_vector_2d.hxx>
VGL_VECTOR_2D_INSTANTIATE(unsigned int);
