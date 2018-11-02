#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "boxm2_vecf_articulated_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_event.h>
typedef std::vector<boxm2_vecf_articulated_params>::iterator iterator;

class boxm2_vecf_scene_articulation{

 public:
  boxm2_vecf_scene_articulation()= default;;
  virtual ~boxm2_vecf_scene_articulation()= default;;

  virtual unsigned size()=0;
  virtual boxm2_vecf_articulated_params& operator[] (unsigned i)=0;

  virtual bool set_play_sequence(std::string seq_id)=0;
  //
  // keys currently used generically  c d v r x y z t ?//
  // add s  toggle transparent skin
  // add m  toggle mouth display
  //
  virtual bool handle(vgui_event const &e){return false;}
};
