#include <vil1/vil1_rgb.txx>

#if 0
#if !VCL_HAS_MEMBER_TEMPLATES
typedef vil1_rgb<double> vil1_rgb_double;

VCL_DECLARE_SPECIALIZATION(vil1_rgb<unsigned char>::vil1_rgb(const vil1_rgb_double& r))
VCL_DEFINE_SPECIALIZATION
vil1_rgb<unsigned char>::vil1_rgb(const vil1_rgb_double& r) {
  double r_R = r.r;
  double r_G = r.g;
  double r_B = r.b;
  double d_R = (r_R-(int)r_R < 0.5) ? r_R : r_R+=1;
  double d_G = (r_G-(int)r_G < 0.5) ? r_G : r_G+=1;
  double d_B = (r_B-(int)r_B < 0.5) ? r_B : r_B+=1;
  r = (unsigned char) d_R;
  g = (unsigned char) d_G;
  b = (unsigned char) d_B;
}
#endif
#endif // 0

VCL_DEFINE_SPECIALIZATION
vcl_ostream& operator<<(vcl_ostream& s, const vil1_rgb<unsigned char>& rgb)
{
 return s << '[' << (int)rgb.r << ' ' << (int)rgb.g << ' ' << (int)rgb.b << ']';
}

VIL1_RGB_INSTANTIATE(unsigned char);
// VIL1_RGB_INSTANTIATE_LS(unsigned char);
