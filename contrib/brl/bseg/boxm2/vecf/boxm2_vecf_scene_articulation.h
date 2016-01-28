#pragma once
#include <vcl_vector.h>
#include "boxm2_vecf_articulated_params.h"
#include <vcl_string.h>
typedef vcl_vector<boxm2_vecf_articulated_params>::iterator iterator;
class boxm2_vecf_scene_articulation
{

public:
  boxm2_vecf_scene_articulation() {};
  virtual ~boxm2_vecf_scene_articulation() {};

  virtual unsigned size() = 0;

  virtual boxm2_vecf_articulated_params & operator[](unsigned i) = 0;

  virtual bool set_play_sequence(vcl_string seq_id) = 0;

};
