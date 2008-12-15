// This is brl/bseg/brec/pro/brec_prob_map_roc_compute_process.h
#ifndef brec_prob_map_roc_compute_process_h_
#define brec_prob_map_roc_compute_process_h_
//:
// \file
// \brief A class to generate TPR and FPR for a given prob map and
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli adapted from Joseph Mundy's bbgm_roc_compute_process
// \date Sep 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_prob_map_roc_compute_process : public bprb_process
{
 public:

  brec_prob_map_roc_compute_process();

  //: Copy Constructor (no local data)
  brec_prob_map_roc_compute_process(const brec_prob_map_roc_compute_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_prob_map_roc_compute_process(){};

  //: Clone the process
  virtual brec_prob_map_roc_compute_process* clone() const {return new brec_prob_map_roc_compute_process(*this);}

  vcl_string name(){return "brecProbMapROCProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_prob_map_roc_compute_process_h_
