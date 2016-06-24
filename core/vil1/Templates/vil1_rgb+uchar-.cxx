#include <vil1/vil1_rgb.hxx>

template <>
std::ostream& operator<<(std::ostream& s, const vil1_rgb<unsigned char>& rgb)
{
 return s << '[' << (int)rgb.r << ' ' << (int)rgb.g << ' ' << (int)rgb.b << ']';
}

VIL1_RGB_INSTANTIATE(unsigned char);
// VIL1_RGB_INSTANTIATE_LS(unsigned char);
