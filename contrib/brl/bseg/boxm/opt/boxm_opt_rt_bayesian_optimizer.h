#ifndef boxm_opt_rt_bayesian_optimizer_h_
#define boxm_opt_rt_bayesian_optimizer_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include "boxm_aux_traits.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/opt/boxm_rt_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_opt_rt_bayesian_optimizer
{
public:

  boxm_opt_rt_bayesian_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, 
                                 vcl_vector<vcl_string> const& image_ids);

  ~boxm_opt_rt_bayesian_optimizer(){}

  bool optimize_cells(double damping_factor);

protected:

  vcl_vector<vcl_string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  const float max_cell_P_;
  const float min_cell_P_;

};















#endif
