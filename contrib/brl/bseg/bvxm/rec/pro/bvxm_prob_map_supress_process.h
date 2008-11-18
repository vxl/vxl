// This is brl/bseg/bvxm/rec/pro/bvxm_prob_map_supress_process.h
#ifndef bvxm_prob_map_supress_process_h_
#define bvxm_prob_map_supress_process_h_
//:
// \file
// \brief A class to suppress a given density/prob map with respect to another map, e.g. to remove vehicle areas from a change map
//
// Input map is P(X in B), suppressor is e.g. P(X in V) (x is a vehicles pixel)
// Output of this process is: P(X in B and X not in V)
//
// \author Ozge Can Ozcanli
// \date Oct. 28, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_prob_map_supress_process : public bprb_process
{
 public:

  bvxm_prob_map_supress_process();

  //: Copy Constructor (no local data)
  bvxm_prob_map_supress_process(const bvxm_prob_map_supress_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_prob_map_supress_process(){};

  //: Clone the process
  virtual bvxm_prob_map_supress_process* clone() const {return new bvxm_prob_map_supress_process(*this);}

  vcl_string name() { return "bvxmProbMapSupressProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_prob_map_supress_process_h_
