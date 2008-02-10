#include "bbgm_loader.h"
#include "bbgm_image_of.h"
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>

bool bbgm_loader::registered_ = false;

void bbgm_loader::register_loaders(){
  if(registered_) return;
  typedef bsta_num_obs<bsta_gauss_f1> sph_gauss_type;
  typedef bsta_num_obs<bsta_mixture<sph_gauss_type> > sph_mix_gauss_type;
  vsl_add_to_binary_loader(bbgm_image_of<sph_mix_gauss_type>());

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture_fixed<gauss_type,3> mix_gauss_type_fixed;
  typedef bsta_num_obs<mix_gauss_type_fixed> obs_mix_gauss_type_fixed;
  bbgm_image_of<obs_mix_gauss_type_fixed> bif;
  vsl_add_to_binary_loader(bif);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;
  bbgm_image_of<mix_gauss_type> bi;
  vsl_add_to_binary_loader(bi);
  registered_ = true;
}
