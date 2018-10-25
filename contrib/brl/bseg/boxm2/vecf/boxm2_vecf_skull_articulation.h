#ifndef boxm2_vecf_skull_articulation_h_
#define boxm2_vecf_skull_articulation_h_
//:
// \file
// \brief  Articulation for the skull model
//
// \author J.L. Mundy
// \date   8 November 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_skull_params.h"

class boxm2_vecf_skull_articulation : public boxm2_vecf_scene_articulation{
 public:
  boxm2_vecf_skull_articulation();
  ~boxm2_vecf_skull_articulation() override{
    params_.clear();
  }

 unsigned size() override{ return static_cast<unsigned>(params_.size()); }

  bool set_play_sequence(std::string seq_id) override{
   if(play_sequence_map_.find(seq_id) != play_sequence_map_.end()){
     params_ = play_sequence_map_[seq_id];
     return true;
   }else{
     std::cout<<"could not find play sequance "<<seq_id<<" in map. choosing default one"<<std::endl;
     params_=play_sequence_map_["default"];
     return false;
   }
  }
 boxm2_vecf_articulated_params& operator [] (unsigned i) override {
   return params_[i] ; }
 private:
  std::vector<boxm2_vecf_skull_params> params_;
  std::map<std::string,std::vector<boxm2_vecf_skull_params> > play_sequence_map_;

};
#endif// boxm2_vecf_skull_articulation
