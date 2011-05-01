#ifndef boxm_shadow_app_initializer_h_
#define boxm_shadow_app_initializer_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_rt_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_shadow_app_initializer
{
public:

  boxm_shadow_app_initializer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, 
                              vcl_vector<vcl_string> const& image_ids,
                              float min_app_sigma,
                              float shadow_prior,
                              float shadow_mean, float shadow_sigma,
                              bool verbose=false);

  ~boxm_shadow_app_initializer(){}

  bool initialize();

protected:

  vcl_vector<vcl_string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  float min_app_sigma_;
  float shadow_prior_;
  float shadow_mean_;
  float shadow_sigma_;
  bool verbose_;
};
#endif
