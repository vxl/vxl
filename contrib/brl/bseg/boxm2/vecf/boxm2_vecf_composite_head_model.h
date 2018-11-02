#ifndef boxm2_vecf_composite_head_model_h_
#define boxm2_vecf_composite_head_model_h_

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_eye_scene.h"
#include "boxm2_vecf_orbit_scene.h"
#include "boxm2_vecf_head_model.h"
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_composite_head_parameters.h"
#include "boxm2_vecf_articulated_scene.h"

class boxm2_vecf_composite_head_model : public boxm2_vecf_head_model{
friend class boxm2_vecf_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
public:
  boxm2_vecf_composite_head_model(std::string const& head_model_path, std::string const& eye_model_path );

  void map_to_target(boxm2_scene_sptr target) override;

  bool set_params(boxm2_vecf_articulated_params const& params) override;
  boxm2_vecf_composite_head_parameters const& get_params() const {return params_;}
private:
  boxm2_vecf_composite_head_parameters params_;

  boxm2_vecf_eye_scene right_eye_;
  boxm2_vecf_eye_scene left_eye_;

  boxm2_vecf_orbit_scene right_orbit_;
  boxm2_vecf_orbit_scene left_orbit_;
  std::string scene_path;
};

#endif
