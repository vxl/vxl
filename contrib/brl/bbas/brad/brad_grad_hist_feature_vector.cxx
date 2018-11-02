#include "brad_grad_hist_feature_vector.h"
//
#include <bsta/bsta_histogram.h>
#include <vil/vil_image_view.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vnl_vector<double> brad_grad_hist_feature_vector::
operator()(vil_image_view<float> const& view) const
{
  bsta_histogram<float> h(min_, max_, nbins_);
  int ni = view.ni(), nj = view.nj();
  assert(ni>=5&&nj>=5);//must have at least one gradient window
  for (int j = 1; j<nj-1; ++j)
    for (int i = 1; i<ni-1; ++i){
      float dx = -(view(i-1, j-1) + 2.0f*view(i-1, j) + view(i-1, j+1));
      dx += view(i+1, j-1) + 2.0f*view(i+1, j) + view(i+1, j+1);
      float dy = -(view(i-1, j-1) + 2.0f*view(i, j-1) + view(i+1, j-1));
      dy += view(i-1, j+1) + 2.0f*view(i, j+1) + view(i+1, j+1);
      float mag = std::sqrt(dx*dx + dy*dy);
      mag/=6.0f;
      h.upcount(mag, 1.0f);
    }
  vnl_vector<double> ret(nbins_+1);
  for (unsigned b = 0; b<nbins_; ++b)
    ret[b]=h.p(b);
  ret[nbins_]= h.entropy();
  return ret;
}

void vsl_b_write(vsl_b_ostream &os, const brad_grad_hist_feature_vector& fv)
{
  vsl_b_write(os, fv.min());
  vsl_b_write(os, fv.max());
  vsl_b_write(os, fv.nbins());
}

void vsl_b_read(vsl_b_istream &is, brad_grad_hist_feature_vector& fv)
{
  float min, max;
  unsigned nbins;
  vsl_b_read(is, min);
  vsl_b_read(is, max);
  vsl_b_read(is, nbins);
  fv.set_nbins(nbins);
  fv.set_min(min);
  fv.set_max(max);
}

void vsl_print_summary(std::ostream &os, brad_grad_hist_feature_vector& fv)
{
  fv.print(os);
}
