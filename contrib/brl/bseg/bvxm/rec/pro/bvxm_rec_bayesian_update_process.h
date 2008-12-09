// This is brl/bseg/bvxm/rec/pro/bvxm_rec_bayesian_update_process.h
#ifndef bvxm_rec_bayesian_update_process_h_
#define bvxm_rec_bayesian_update_process_h_
//:
// \file
// \brief A class to apply bayesian rule to find posterior probability of an entity given a measurement map based on background and foreground models of the given entity
//                      e.g. given a map P(Entity) and a measurement map: M.
//                      and given models: P(Measurement | Entity) and P(Measurement | not Entity)
//                      return P(Entity | Measurement)
//        for instance: given expected area difference map (measurement) for "foreground glitches on background" (entity)
//                      return glitch detection map
//          
// \author Ozge Can Ozcanli
// \date Apr 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_rec_bayesian_update_process : public bprb_process
{
 public:

  bvxm_rec_bayesian_update_process();

  //: Copy Constructor (no local data)
  bvxm_rec_bayesian_update_process(const bvxm_rec_bayesian_update_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_rec_bayesian_update_process(){};

  //: Clone the process
  virtual bvxm_rec_bayesian_update_process* clone() const {return new bvxm_rec_bayesian_update_process(*this);}

  //vcl_string name(){return "bvxmUpdateChangesWrtAreaProcess";}
  vcl_string name(){return "bvxmRecBayesianUpdateProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_rec_bayesian_update_process_h_
