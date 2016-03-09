#include "brad_hist_prob_feature_vector.h"
//
#include <bsta/bsta_histogram.h>
#include <vil/vil_image_view.h>


vnl_vector<double> brad_hist_prob_feature_vector::
operator()(vil_image_view<float> const& view) const
{
  bsta_histogram<float> h(min_, max_, nbins_);
  unsigned ni = view.ni(), nj = view.nj();
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      h.upcount(view(i,j), 1.0f);
  vnl_vector<double> ret(nbins_+1);
  for (unsigned b = 0; b<nbins_; ++b)
    ret[b]=h.p(b);
  ret[nbins_]= h.entropy();
  return ret;
}

void vsl_b_write(vsl_b_ostream &os, const brad_hist_prob_feature_vector& fv)
{
  vsl_b_write(os, fv.min());
  vsl_b_write(os, fv.max());
  vsl_b_write(os, fv.nbins());
}

void vsl_b_read(vsl_b_istream &is, brad_hist_prob_feature_vector& fv)
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

void vsl_print_summary(std::ostream &os, brad_hist_prob_feature_vector& fv)
{
  fv.print(os);
}
