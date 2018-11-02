#ifndef boxm_shadow_app_initializer_h_
#define boxm_shadow_app_initializer_h_

#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_rt_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_shadow_app_initializer
{
public:

  boxm_shadow_app_initializer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                              std::vector<std::string>  image_ids,
                              float min_app_sigma,
                              float shadow_prior,
                              float shadow_mean, float shadow_sigma,
                              bool verbose=false);

  ~boxm_shadow_app_initializer()= default;

  bool initialize();

protected:

  std::vector<std::string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  float min_app_sigma_;
  float shadow_prior_;
  float shadow_mean_;
  float shadow_sigma_;
  bool verbose_;
};
#endif
