#ifndef boxm_opt2_bayesian_optimizer_h_
#define boxm_opt2_bayesian_optimizer_h_

#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_opt2_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_opt2_optimizer
{
 public:
  boxm_opt2_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                      std::vector<std::string>  image_ids);

  ~boxm_opt2_optimizer()= default;

  bool update();

 protected:
  std::vector<std::string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  const float max_cell_P_;
  const float min_cell_P_;
};

#endif
