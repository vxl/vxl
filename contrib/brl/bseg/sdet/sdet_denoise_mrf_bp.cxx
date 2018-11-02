// This is brl/bseg/sdet/sdet_denoise_mrf_bp.cxx
#include <iostream>
#include <cstdlib>
#include "sdet_denoise_mrf_bp.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_line_generator.h>

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

// constructor from a parameter block (the only way)
sdet_denoise_mrf_bp::sdet_denoise_mrf_bp(sdet_denoise_mrf_bp_params& dmp)
  : sdet_denoise_mrf_bp_params(dmp), output_valid_(false), use_var_(false)
{}

// Default Destructor
sdet_denoise_mrf_bp::~sdet_denoise_mrf_bp()
= default;

void sdet_denoise_mrf_bp::
set_image(vil_image_resource_sptr const& resource)
{
  vil_image_view_base_sptr view = resource->get_view();
  pyr_in_ = vil_pyramid_image_view<float>(view, pyramid_levels_);
}

void sdet_denoise_mrf_bp::set_variance(vil_image_resource_sptr const& var_resc)
{
  vil_image_view_base_sptr var_view = var_resc->get_view();
  pyr_var_ = vil_pyramid_image_view<float>(var_view, pyramid_levels_);
  use_var_ = true;
}


bool sdet_denoise_mrf_bp::denoise()
{
  int lev = static_cast<int>(pyramid_levels_)-1;
  vil_image_view<float> depth = pyr_in_(lev);
  if (use_var_) {
    vil_image_view<float> var = pyr_var_(lev);
    mrf_ = new sdet_mrf_bp(depth, var, n_labels_, discontinuity_cost_,
                           truncation_cost_, kappa_, lambda_);
  }
  else
    mrf_ = new sdet_mrf_bp(depth, n_labels_, discontinuity_cost_,
                           truncation_cost_, kappa_, lambda_);

  vul_timer t;
  for (unsigned it = 0; it<n_iter_; ++it) {
    mrf_->send_messages_optimized();
    std::cout << '.'<<std::flush;
  }
  std::cout << "completed belief propagation at top level in "
           << t.real()/1000.0 << " seconds\n";

  for (--lev ; lev>=0; --lev) {
    auto pre_lev = static_cast<unsigned>(lev+1);
    mrf_ = pyramid_upsample(mrf_, pre_lev);
    t.mark();
    for (unsigned it = 0; it<n_iter_; ++it) {
      mrf_->send_messages_optimized();
      std::cout << '.'<<std::flush;
    }
    std::cout << "completed belief propagation at pyramid level " << lev
             << " in " << t.real()/1000.0 << " seconds\n";
  }
  output_valid_ = true;
    out_resc_ = mrf_->belief_image();

  return true;
}

sdet_mrf_bp_sptr sdet_denoise_mrf_bp::
pyramid_upsample(sdet_mrf_bp_sptr const& in_mrf, unsigned level)
{
  if (level==0)
    return nullptr;
  unsigned nj = in_mrf->nj(), ni = in_mrf->ni();
  if (!ni || !nj) return nullptr;
  //initialize a mrf at the next resolution level
  vil_image_view<float> in_view = pyr_in_(level-1);
  unsigned njd = in_view.nj(), nid = in_view.ni();
   sdet_mrf_bp_sptr out_mrf;
  if (use_var_) {
    vil_image_view<float> var_view = pyr_var_(level-1);
    out_mrf= new sdet_mrf_bp(in_view, var_view, n_labels_,
                             discontinuity_cost_,
                             truncation_cost_,
                             kappa_, lambda_);
  }
  else
    out_mrf = new sdet_mrf_bp(in_view, n_labels_,
                              discontinuity_cost_,
                              truncation_cost_,
                              kappa_, lambda_);
  for (unsigned n = 0; n<4; ++n)
    for (unsigned j = 0; j<nj; ++j) {
      unsigned jn = 2*j;
      if (jn+1>=njd) continue;
      for (unsigned i = 0; i<ni; ++i)
      {
        unsigned in = 2*i;
        if (in+1>=nid) continue;
        std::vector<float> msg = in_mrf->prior_message(i, j, n);
        out_mrf->set_prior_message(in, jn, n, msg);
        out_mrf->set_prior_message(in+1, jn, n, msg);
        out_mrf->set_prior_message(in, jn+1, n, msg);
        out_mrf->set_prior_message(in+1, jn+1, n, msg);
      }
    }
  return out_mrf;
}
