#include "brad_grad_int_feature_vector.h"
//
#include <bsta/bsta_histogram.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vnl_vector<double> brad_grad_int_feature_vector::
operator()(vil_image_view<float> const& view) const
{
  bsta_histogram<float> h_int(min_int_, max_int_, nbins_);
  bsta_histogram<float> h_grad(min_grad_, max_grad_, nbins_);
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
      h_grad.upcount(mag, 1.0f);
      h_int.upcount(view(i,j), 1.0f);
    }
  vnl_vector<double> ret(2*nbins_+2);
  for (unsigned b = 0; b<nbins_; ++b){
    ret[b]=h_int.p(b);
    ret[b+nbins_+1] = h_grad.p(b);
  }
  auto l2e = static_cast<float>(vnl_math::log2e);
  auto nb = static_cast<float>(nbins_);
  float max_entropy = l2e*std::log(nb);
  ret[nbins_]= h_int.entropy()/max_entropy;
  ret[2*nbins_+1]= h_grad.entropy()/max_entropy;
  return ret;
}

void vsl_b_write(vsl_b_ostream &os, const brad_grad_int_feature_vector& fv)
{
  vsl_b_write(os, fv.min_int());
  vsl_b_write(os, fv.max_int());
  vsl_b_write(os, fv.min_grad());
  vsl_b_write(os, fv.max_grad());
  vsl_b_write(os, fv.nbins());
}

void vsl_b_read(vsl_b_istream &is, brad_grad_int_feature_vector& fv)
{
  float min_int, max_int;
  float min_grad, max_grad;
  unsigned nbins;
  vsl_b_read(is, min_int);
  vsl_b_read(is, max_int);
  vsl_b_read(is, min_grad);
  vsl_b_read(is, max_grad);
  vsl_b_read(is, nbins);
  fv.set_nbins(nbins);
  fv.set_min_int(min_int);
  fv.set_max_int(max_int);
  fv.set_min_grad(min_grad);
  fv.set_max_grad(max_grad);
}

void vsl_print_summary(std::ostream &os, brad_grad_int_feature_vector& fv)
{
  fv.print(os);
}
