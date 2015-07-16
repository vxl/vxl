#pragma once
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_orbit_scene.h>
#include <boxm2/vecf/boxm2_vecf_composite_head_model.h>
#include <boxm2/boxm2_scene.h>
class boxm2_vecf_appearance_extractor{
 public:
 boxm2_vecf_appearance_extractor(boxm2_scene_sptr target_scene, boxm2_vecf_composite_head_model& source_model):source_model_(source_model){
    target_scene_ = target_scene;
  }
  void extract_appearance(){
    this->extract_orbit_appearance();
    this->extract_head_appearance();
    source_model_.intrinsic_change_ = true;
  }
 private:
  void extract_orbit_appearance();
  void extract_head_appearance();
  boxm2_scene_sptr target_scene_;
  boxm2_vecf_composite_head_model& source_model_;
};
