#ifndef boxm2_vecf_composite_head_model_articulation_h_
#define boxm2_vecf_composite_head_model_articulation_h_
//:
// \file
// \brief  Parameters for the composite_head_model model
//
// \author Octi Biris
// \date   9 Jul 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_composite_head_parameters.h"

class boxm2_vecf_composite_head_model_articulation : public boxm2_vecf_scene_articulation{
 public:
  //default constructor creates play sequences around a default set of composite head params
  boxm2_vecf_composite_head_model_articulation(){
    boxm2_vecf_composite_head_parameters default_params;
    this->register_play_sequences(default_params);
    this->set_play_sequence("default");
  }
  // constructor to create the play sequences that modify a specific set of parameters
  boxm2_vecf_composite_head_model_articulation( boxm2_vecf_composite_head_parameters const& head_params){
    this->register_play_sequences(head_params);
    this->set_play_sequence("default");
  }

  ~boxm2_vecf_composite_head_model_articulation() override{
    params_.clear();
  }

 unsigned size() override{ return static_cast<unsigned>(params_.size()); }
 boxm2_vecf_articulated_params& operator [] (unsigned i) override {
   return params_[i] ; }
  void register_play_sequences(boxm2_vecf_composite_head_parameters const&  head_params);

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
 private:
  std::vector<boxm2_vecf_composite_head_parameters> params_;
  std::map<std::string,std::vector<boxm2_vecf_composite_head_parameters> > play_sequence_map_;

};
#endif// boxm2_vecf_composite_head_model_articulation
