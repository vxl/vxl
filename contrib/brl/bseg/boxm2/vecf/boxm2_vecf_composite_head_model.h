#ifndef boxm2_vecf_composite_head_model_h_
#define boxm2_vecf_composite_head_model_h_

#include <vcl_string.h>

#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_eye_scene.h"
#include "boxm2_vecf_orbit_scene.h"
#include "boxm2_vecf_head_model.h"
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_composite_head_parameters.h"
#include "boxm2_vecf_articulated_scene.h"
class boxm2_vecf_composite_head_model : public boxm2_vecf_head_model{
public:
  boxm2_vecf_composite_head_model(vcl_string const& head_model_path, vcl_string const& eye_model_path );

  void map_to_target(boxm2_scene_sptr target);

  bool set_params(boxm2_vecf_articulated_params const& params);
  void get_params(boxm2_vecf_composite_head_parameters &params) const {params = params_;}

private:
  boxm2_vecf_composite_head_parameters params_;

  boxm2_vecf_eye_scene right_eye_;
  boxm2_vecf_eye_scene left_eye_;

  boxm2_vecf_orbit_scene right_orbit_;
  boxm2_vecf_orbit_scene left_orbit_;
  vcl_string scene_path;
  bool use_orbit;

};

#endif
