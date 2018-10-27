#ifndef boxm2_vecf_middle_fat_pocket_articulation_h_
#define boxm2_vecf_middle_fat_pocket_articulation_h_
//:
// \file
// \brief  Articulation for the middle_fat_pocket model
//
// \author J.L. Mundy
// \date   24 January 2015
//
#include <string>
#include <vector>
#include <map>
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_middle_fat_pocket_params.h"
class boxm2_vecf_middle_fat_pocket_articulation : public boxm2_vecf_scene_articulation{
 public:
  boxm2_vecf_middle_fat_pocket_articulation();
  ~boxm2_vecf_middle_fat_pocket_articulation() override{
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
  std::vector<boxm2_vecf_middle_fat_pocket_params> params_;
  std::map<std::string,std::vector<boxm2_vecf_middle_fat_pocket_params> > play_sequence_map_;
};
#endif// boxm2_vecf_middle_fat_pocket_articulation
