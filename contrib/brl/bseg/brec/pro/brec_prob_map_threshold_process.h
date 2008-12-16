// This is brl/bseg/brec/pro/brec_prob_map_threshold_process.h
#ifndef brec_prob_map_threshold_process_h_
#define brec_prob_map_threshold_process_h_
//:
// \file
// \brief A class to generate a thresholded image for a given prob map and threshold value
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli
// \date 11/06/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_prob_map_threshold_process : public bprb_process
{
 public:

  brec_prob_map_threshold_process();

  //: Copy Constructor (no local data)
  brec_prob_map_threshold_process(const brec_prob_map_threshold_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_prob_map_threshold_process(){};

  //: Clone the process
  virtual brec_prob_map_threshold_process* clone() const {return new brec_prob_map_threshold_process(*this);}

  vcl_string name(){return "brecProbMapThresholdProcess";}

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};


#endif // brec_prob_map_threshold_process_h_
