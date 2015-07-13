#ifndef boxm2_vecf_head_model_h_
#define boxm2_vecf_head_model_h_
//:
// \file
// \brief  boxm2_vecf_head_model static model of human head (no eyes)
//
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include "boxm2_vecf_articulated_scene.h"
#include "boxm2_vecf_articulated_params.h"
class boxm2_vecf_head_model : public boxm2_vecf_articulated_scene
{
 public:
  boxm2_vecf_head_model(vcl_string const& scene_file);

  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene);

  bool clear_target(boxm2_scene_sptr target_scene);

  void set_scale(vgl_vector_3d<double> scale);

  virtual bool set_params(boxm2_vecf_articulated_params const& params){return true;} //blank for now
 protected:
  bool intrinsic_change_;
private:
  vgl_vector_3d<double> scale_;
  bool get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                boxm2_data_base **alpha_data,
                boxm2_data_base **app_data,
                boxm2_data_base **nobs_data);


};
#endif // boxm2_vecf_head_model_h_
