#include <vil/vil_rgb.txx>
#include <vxl_config.h>

VCL_DEFINE_SPECIALIZATION
vcl_ostream& operator<<(vcl_ostream& s, vil_rgb<vxl_byte> const& rgb)
{
 return s << '[' << (int)rgb.r << ' ' << (int)rgb.g << ' ' << (int)rgb.b << ']';
}

VIL_RGB_INSTANTIATE( vxl_byte );
