// This is brl/bseg/brec/pro/brec_density_to_prob_map_process.h
#ifndef brec_density_to_prob_map_process_h_
#define brec_density_to_prob_map_process_h_
//:
// \file
// \brief A class to convert a prob density image into a probability map, output will be of type float with values in [0,1]
//
//  CAUTION: Input image is assumed to have type float (not necessarily with values in [0,1])
//
// \author Ozge Can Ozcanli
// \date 09/01/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_density_to_prob_map_process : public bprb_process
{
 public:

  brec_density_to_prob_map_process();

  //: Copy Constructor (no local data)
  brec_density_to_prob_map_process(const brec_density_to_prob_map_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_density_to_prob_map_process(){};

  //: Clone the process
  virtual brec_density_to_prob_map_process* clone() const {return new brec_density_to_prob_map_process(*this);}

  vcl_string name(){return "brecDensityToProbMapProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_density_to_prob_map_process_h_
