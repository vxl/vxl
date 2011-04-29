// Disable warning
#include <vcl_compiler.h>
#ifdef VCL_VC_DOTNET
// 4146: unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4146)
#endif //VCL_VC_DOTNET

// Instantiation of vgl_homg_point_2d<unsigned>
#include <vgl/vgl_homg_point_2d.txx>
VGL_HOMG_POINT_2D_INSTANTIATE(unsigned);
