#include "bmrf_epi_seg_compare.h"
//:
// \file



//: Compute the average distance ratio in the overlapping alpha
double
bmrf_avg_distance_ratio( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2)
{
  double min_alpha = bmrf_min_alpha(ep1, ep2);
  double max_alpha = bmrf_max_alpha(ep1, ep2);
  double d_alpha =   bmrf_d_alpha(ep1, ep2);

  double s1 = 0.0, s2 = 0.0;
  for (double alpha = min_alpha; alpha <= max_alpha; alpha += d_alpha) {
    s1 += ep1->s(alpha) * ep1->s(alpha); // s1(alpha)^2
    s2 += ep2->s(alpha) * ep1->s(alpha); // s1(alpha) * s2(alpha)
  }
  return s1 / s2;
}


//: Compute the average match error between to segments
double
bmrf_match_error( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2 )
{
  double min_alpha = bmrf_min_alpha(ep1, ep2);
  double max_alpha = bmrf_max_alpha(ep1, ep2);
  double d_alpha =   bmrf_d_alpha(ep1, ep2);

  double s_error = 0.0;
  for (double alpha = min_alpha; alpha <= max_alpha; alpha += d_alpha) {
    double ds = ep1->s(alpha) - ep2->s(alpha);
    s_error += ds*ds;
  }
  return s_error * d_alpha / (max_alpha - min_alpha);
}


//: Compute the average intensity error between to segments
double
bmrf_intensity_error( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2 )
{
  double min_alpha = bmrf_min_alpha(ep1, ep2);
  double max_alpha = bmrf_max_alpha(ep1, ep2);
  double d_alpha =   bmrf_d_alpha(ep1, ep2);

  double l_error = 0.0, r_error = 0.0;
  for (double alpha = min_alpha; alpha <= max_alpha; alpha += d_alpha)
  {
    double dli = (ep1->left_int(alpha) - ep2->left_int(alpha));
    double dri = (ep1->right_int(alpha) - ep2->right_int(alpha));
    l_error += dli*dli;
    r_error += dri*dri;
  }
  return  (l_error + r_error) * d_alpha / (max_alpha - min_alpha);
}
