#ifndef boxm2_vecf_cranium_articulation_h_
#define boxm2_vecf_cranium_articulation_h_
//:
// \file
// \brief  Parameters for the cranium model
//
// \author J.L. Mundy
// \date   7 Nov 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_cranium_params.h"

class boxm2_vecf_cranium_articulation : public boxm2_vecf_scene_articulation{
 public:
  boxm2_vecf_cranium_articulation();
  ~boxm2_vecf_cranium_articulation(){
    params_.clear();
  }

 unsigned size(){ return static_cast<unsigned>(params_.size()); }

  bool set_play_sequence(vcl_string seq_id){
   if(play_sequence_map_.find(seq_id) != play_sequence_map_.end()){
     params_ = play_sequence_map_[seq_id];
     return true;
   }else{
     vcl_cout<<"could not find play sequance "<<seq_id<<" in map. choosing default one"<<vcl_endl;
     params_=play_sequence_map_["default"];
     return false;
   }
  }
 boxm2_vecf_articulated_params& operator [] (unsigned i) {
   return params_[i] ; }
 private:
  vcl_vector<boxm2_vecf_cranium_params> params_;
  vcl_map<vcl_string,vcl_vector<boxm2_vecf_cranium_params> > play_sequence_map_;


};
#endif// boxm2_vecf_cranium_articulation
