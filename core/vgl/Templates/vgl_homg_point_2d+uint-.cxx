// Disable warning
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#ifdef _MSC_VER
// 4146: unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4146)
#endif //_MSC_VER

// Instantiation of vgl_homg_point_2d<unsigned>
#include <vgl/vgl_homg_point_2d.hxx>
VGL_HOMG_POINT_2D_INSTANTIATE(unsigned);
