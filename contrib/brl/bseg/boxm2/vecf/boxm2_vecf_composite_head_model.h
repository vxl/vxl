#ifndef boxm2_vecf_composite_head_model_h_
#define boxm2_vecf_composite_head_model_h_

#include <vcl_string.h>

#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_eye_scene.h"
#include "boxm2_vecf_head_model.h"
#include "boxm2_vecf_composite_head_parameters.h"

class boxm2_vecf_composite_head_model
{
public:
  boxm2_vecf_composite_head_model(vcl_string const& head_model_path, vcl_string const& eye_model_path );

  bool composite(boxm2_scene_sptr target);

  void set_parameters(boxm2_vecf_composite_head_parameters const& params);

private:
  boxm2_vecf_composite_head_parameters params_;

  boxm2_vecf_eye_scene right_eye_;
  boxm2_vecf_eye_scene left_eye_;
  boxm2_vecf_head_model head_;

};

#endif
