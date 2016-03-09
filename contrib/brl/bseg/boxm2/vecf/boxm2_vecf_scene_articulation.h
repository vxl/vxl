#pragma once
#include <vector>
#include "boxm2_vecf_articulated_params.h"
#include <vcl_compiler.h>
#include <iostream>
#include <string>
typedef std::vector<boxm2_vecf_articulated_params>::iterator iterator;
class boxm2_vecf_scene_articulation{

 public:
  boxm2_vecf_scene_articulation(){};
  virtual ~boxm2_vecf_scene_articulation(){};

  virtual unsigned size()=0;
  virtual boxm2_vecf_articulated_params& operator[] (unsigned i)=0;
  virtual bool set_play_sequence(std::string seq_id)=0;
};
