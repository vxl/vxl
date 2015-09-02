#ifndef boxm2_vecf_ocl_composite_head_model_h_
#define boxm2_vecf_ocl_composite_head_model_h_

#include <vcl_string.h>

#include <boxm2/boxm2_scene.h>
#include "../boxm2_vecf_orbit_scene.h"
#include "boxm2_vecf_ocl_head_model.h"
#include "../boxm2_vecf_articulated_params.h"
#include "../boxm2_vecf_composite_head_parameters.h"
#include "../boxm2_vecf_articulated_scene.h"

class boxm2_vecf_ocl_composite_head_model : public boxm2_vecf_ocl_head_model{
friend class boxm2_vecf_ocl_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
public:
  boxm2_vecf_ocl_composite_head_model(vcl_string const& head_model_path, vcl_string const& eye_model_path,bocl_device_sptr device,boxm2_opencl_cache_sptr opencl_cache);

  void map_to_target(boxm2_scene_sptr target);
  void update_gpu_target(boxm2_scene_sptr target_scene);
  bool set_params(boxm2_vecf_articulated_params const& params);
  boxm2_vecf_composite_head_parameters const& get_params() const {return params_;}
private:
  boxm2_vecf_composite_head_parameters params_;

  boxm2_vecf_orbit_scene right_orbit_;
  boxm2_vecf_orbit_scene left_orbit_;
  vcl_string scene_path;
};

#endif
