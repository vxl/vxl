// Disable warning
#include <vcl_compiler.h>
#ifdef VCL_VC_DOTNET
// 4146: unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4146)
#endif //VCL_VC_DOTNET

// Instantiation of vgl_vector_3d<unsigned int>
#include <vgl/vgl_vector_3d.txx>
VGL_VECTOR_3D_INSTANTIATE(unsigned int);
