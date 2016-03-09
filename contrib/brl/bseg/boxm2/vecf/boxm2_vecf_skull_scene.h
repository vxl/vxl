#ifndef boxm2_vecf_skull_scene_h_
#define boxm2_vecf_skull_scene_h_

#include <vcl_compiler.h>
#include <iostream>
#include <string>

#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_mandible_scene.h"
#include "boxm2_vecf_cranium_scene.h"
#include "boxm2_vecf_head_model.h"
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_skull_params.h"
#include "boxm2_vecf_articulated_scene.h"

class boxm2_vecf_skull_scene : public boxm2_vecf_articulated_scene{

public:
  boxm2_vecf_skull_scene(std::string const& scene_path, std::string const& geo_path);
  ~boxm2_vecf_skull_scene(){delete mandible_scene_; delete cranium_scene_;}
  void map_to_target(boxm2_scene_sptr target);

  bool set_params(boxm2_vecf_articulated_params const& params);
  boxm2_vecf_skull_params const& get_params() const {return params_;}

private:
  bool target_data_extracted_;
  boxm2_vecf_skull_params params_;
  boxm2_vecf_mandible_scene* mandible_scene_;
  boxm2_vecf_cranium_scene* cranium_scene_;
  std::string scene_path;
};

#endif
