#ifndef boxm2_opt2_bayesian_optimizer_h_
#define boxm2_opt2_bayesian_optimizer_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include <boxm2/boxm_aux_traits.h>
#include <boxm2/boxm_apm_traits.h>
#include <boxm2/boxm_scene.h>

#include <boxm2/sample/boxm_opt2_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_opt2_optimizer
{
 public:
  boxm_opt2_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                      vcl_vector<vcl_string> const& image_ids);

  ~boxm_opt2_optimizer(){}

  bool update();

 protected:
  vcl_vector<vcl_string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  const float max_cell_P_;
  const float min_cell_P_;
};

#endif
