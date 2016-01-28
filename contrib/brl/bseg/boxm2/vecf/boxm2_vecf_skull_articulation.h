#ifndef boxm2_vecf_skull_articulation_h_
#define boxm2_vecf_skull_articulation_h_
// :
// \file
// \brief  Articulation for the skull model
//
// \author J.L. Mundy
// \date   8 November 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_skull_params.h"

class boxm2_vecf_skull_articulation : public boxm2_vecf_scene_articulation
{
public:
  boxm2_vecf_skull_articulation();
  ~boxm2_vecf_skull_articulation()
  {
    params_.clear();
  }

  unsigned size() { return static_cast<unsigned>(params_.size() ); }

  bool set_play_sequence(vcl_string seq_id)
  {
    if( play_sequence_map_.find(seq_id) != play_sequence_map_.end() )
      {
      params_ = play_sequence_map_[seq_id];
      return true;
      }
    else
      {
      vcl_cout << "could not find play sequance " << seq_id << " in map. choosing default one" << vcl_endl;
      params_ = play_sequence_map_["default"];
      return false;
      }
  }

  boxm2_vecf_articulated_params & operator [](unsigned i)
  {
    return params_[i];
  }

private:
  vcl_vector<boxm2_vecf_skull_params>                       params_;
  vcl_map<vcl_string, vcl_vector<boxm2_vecf_skull_params> > play_sequence_map_;

};
#endif// boxm2_vecf_skull_articulation
