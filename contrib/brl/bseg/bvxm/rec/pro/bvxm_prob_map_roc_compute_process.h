// This is contrib/bvxm/pro/bvxm_prob_map_roc_compute_process.h
#ifndef bvxm_prob_map_roc_compute_process_h_
#define bvxm_prob_map_roc_compute_process_h_
//:
// \file
// \brief A class to generate TPR and FPR for a given prob map and 
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli adapted from Joseph Mundy's bbgm_roc_compute_process
// \date 09/10/2008
//
// \verbatim
// Modifications 
//
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class bvxm_prob_map_roc_compute_process : public bprb_process
{
 public:

  bvxm_prob_map_roc_compute_process();

  //: Copy Constructor (no local data)
  bvxm_prob_map_roc_compute_process(const bvxm_prob_map_roc_compute_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_prob_map_roc_compute_process(){};

  //: Clone the process
  virtual bvxm_prob_map_roc_compute_process* clone() const {return new bvxm_prob_map_roc_compute_process(*this);}

  vcl_string name(){return "bvxmProbMapROCProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_prob_map_roc_compute_process_h_
