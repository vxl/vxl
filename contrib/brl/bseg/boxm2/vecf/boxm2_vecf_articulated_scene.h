#pragma once
#include <boxm2/boxm2_scene.h>
#include <vcl_string.h>
#include "boxm2_vecf_articulated_params.h"
//smart pointer
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//:
// \file
// \brief  Parent class for articulated anatomy scenes
//
// \author O. T. Biris
// \date   2 Jul 2015
//
class boxm2_vecf_articulated_scene;
typedef vbl_smart_ptr<boxm2_vecf_articulated_scene> boxm2_vecf_articulated_scene_sptr;

class boxm2_vecf_articulated_scene : public vbl_ref_count{


 public:
  boxm2_vecf_articulated_scene(){ base_model_ = 0;}
  boxm2_vecf_articulated_scene(vcl_string scene_file){ base_model_ = new boxm2_scene(scene_file);}
  virtual void set_params(boxm2_vecf_articulated_params const& params)=0;
  virtual void map_to_target(boxm2_scene_sptr target_scene)=0;
  boxm2_scene_sptr scene(){return base_model_;}

 protected:
  boxm2_scene_sptr base_model_;
};
