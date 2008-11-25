// This is brl/bseg/bvxm/rec/pro/bvxm_rec_density_to_prob_map_process.h
#ifndef bvxm_rec_density_to_prob_map_process_h_
#define bvxm_rec_density_to_prob_map_process_h_
//:
// \file
// \brief A class to convert a prob density image into a probability map, output will be of type float with values in [0,1]
//
//  CAUTION: Input image is assumed to have type float (not necessarily with values in [0,1])
//
// \author Ozge Can Ozcanli
// \date Sep 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_rec_density_to_prob_map_process : public bprb_process
{
 public:

  bvxm_rec_density_to_prob_map_process();

  //: Copy Constructor (no local data)
  bvxm_rec_density_to_prob_map_process(const bvxm_rec_density_to_prob_map_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_rec_density_to_prob_map_process(){};

  //: Clone the process
  virtual bvxm_rec_density_to_prob_map_process* clone() const {return new bvxm_rec_density_to_prob_map_process(*this);}

  vcl_string name(){return "bvxmDensityToProbMapProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_rec_density_to_prob_map_process_h_
