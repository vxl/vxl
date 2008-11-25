// This is brl/bseg/bvxm/rec/pro/bvxm_prob_map_area_process.h
#ifndef bvxm_prob_map_area_process_h_
#define bvxm_prob_map_area_process_h_
//:
// \file
// \brief A class to find expected area at each pixel for a given prob map
//
// \author Ozge Can Ozcanli
// \date Nov 06, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_prob_map_area_process : public bprb_process
{
 public:

  bvxm_prob_map_area_process();

  //: Copy Constructor (no local data)
  bvxm_prob_map_area_process(const bvxm_prob_map_area_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_prob_map_area_process(){};

  //: Clone the process
  virtual bvxm_prob_map_area_process* clone() const {return new bvxm_prob_map_area_process(*this);}

  vcl_string name(){return "bvxmProbMapAreaProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_prob_map_area_process_h_
