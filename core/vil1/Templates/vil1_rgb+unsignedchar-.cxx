#include <vcl/vcl_compiler.h>
#include <vil/vil_rgb.txx>

#if !VCL_HAS_MEMBER_TEMPLATES
typedef vil_rgb<double> vil_rgb_double;
 
// VCL_DECxLARE_SPECIALIZATION(vil_rgb<unsigned char>::vil_rgb(const vil_rgb_double& r))
VCL_DEFINE_SPECIALIZATION
vil_rgb<unsigned char>::vil_rgb(const vil_rgb_double& r) {
  double r_R = r.R_;
  double r_G = r.G_;
  double r_B = r.B_;
  double d_R = (r_R-(int)r_R < 0.5) ? r_R : r_R+=1;
  double d_G = (r_G-(int)r_G < 0.5) ? r_G : r_G+=1;
  double d_B = (r_B-(int)r_B < 0.5) ? r_B : r_B+=1;
  R_ = (unsigned char) d_R;
  G_ = (unsigned char) d_G;
  B_ = (unsigned char) d_B;
}
#endif

ostream& operator<<(ostream& s, const vil_rgb<unsigned char>& rgb)
{
 return s << '[' << (int)rgb.R_ << ' ' << (int)rgb.G_ << ' ' << (int)rgb.B_ << ']';
}


VBL_RGB_INSTANTIATE(unsigned char);
VBL_RGB_INSTANTIATE_LS(unsigned char);
