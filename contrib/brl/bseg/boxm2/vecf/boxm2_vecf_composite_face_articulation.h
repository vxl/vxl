#ifndef boxm2_vecf_composite_face_articulation_h_
#define boxm2_vecf_composite_face_articulation_h_
//:
// \file
// \brief  Articulation for the composite_face model
//
// \author J.L. Mundy
// \date   5 November 2015
//
#include <string>
#include <vector>
#include <map>
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_composite_face_params.h"
class boxm2_vecf_composite_face_articulation : public boxm2_vecf_scene_articulation{
 public:
  //:default constructor, uses default parameter constructor
 boxm2_vecf_composite_face_articulation():skin_is_transparent_(false),show_mouth_region_(false){initialize();}

  //: this form of the constructor allows the parameters members that
  // differ from the default parameter constructor that are not modified by the articulation
  // for example the global transformation between the source and target scenes
 boxm2_vecf_composite_face_articulation(boxm2_vecf_composite_face_params const& params):skin_is_transparent_(false),
    show_mouth_region_(false){initial_params_ = params; initialize();}
  ~boxm2_vecf_composite_face_articulation() override{
    params_.clear();
  }
  bool handle(vgui_event const &e) override;
  void set_initial_params(boxm2_vecf_composite_face_params const& params){initial_params_ = params;}
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
 // used by both constructors to fill the params_ sequence
 void initialize();
 boxm2_vecf_composite_face_params initial_params_;
 std::vector<boxm2_vecf_composite_face_params> params_;
  std::map<std::string,std::vector<boxm2_vecf_composite_face_params> > play_sequence_map_;
  bool skin_is_transparent_;
  bool show_mouth_region_;
};
#endif// boxm2_vecf_composite_face_articulation
