// This is brl/bseg/bvxm/rec/pro/bvxm_prob_map_threshold_process.h
#ifndef bvxm_prob_map_threshold_process_h_
#define bvxm_prob_map_threshold_process_h_
//:
// \file
// \brief A class to generate a thresholded image for a given prob map and threshold value
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
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

class bvxm_prob_map_threshold_process : public bprb_process
{
 public:

  bvxm_prob_map_threshold_process();

  //: Copy Constructor (no local data)
  bvxm_prob_map_threshold_process(const bvxm_prob_map_threshold_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_prob_map_threshold_process(){};

  //: Clone the process
  virtual bvxm_prob_map_threshold_process* clone() const {return new bvxm_prob_map_threshold_process(*this);}

  vcl_string name(){return "bvxmProbMapThresholdProcess";}

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};


#endif // bvxm_prob_map_threshold_process_h_
