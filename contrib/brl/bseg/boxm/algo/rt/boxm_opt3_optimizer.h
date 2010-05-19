#ifndef boxm_opt3_optimizer_h_
#define boxm_opt3_optimizer_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_rt_sample.h>

template<class T_loc, boxm_apm_type APM, boxm_apm_type AUX_APM>
class boxm_opt3_optimizer
{
 public:
  boxm_opt3_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                      vcl_vector<vcl_string> const& image_ids);

  ~boxm_opt3_optimizer(){}

  bool optimize_cells();

 protected:
  vcl_vector<vcl_string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene_;

  const float max_cell_P_;
  const float min_cell_P_;
};

#endif
