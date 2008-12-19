// This is brl/bseg/brec/pro/brec_bayesian_update_process.h
#ifndef brec_bayesian_update_process_h_
#define brec_bayesian_update_process_h_
//:
// \file
// \brief A class to apply Bayesian rule to find posterior probability of an entity given a measurement map based on background and foreground models of the given entity
//
// e.g. given a map P(Entity) and a measurement map: M.
// and given models: P(Measurement | Entity) and P(Measurement | not Entity)
// return P(Entity | Measurement)
// for instance: given expected area difference map (measurement) for "foreground glitches on background" (entity)
//               return glitch detection map
//
// \author Ozge Can Ozcanli
// \date 10/01/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_bayesian_update_process : public bprb_process
{
 public:

  brec_bayesian_update_process();

  //: Copy Constructor (no local data)
  brec_bayesian_update_process(const brec_bayesian_update_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_bayesian_update_process(){};

  //: Clone the process
  virtual brec_bayesian_update_process* clone() const {return new brec_bayesian_update_process(*this);}

  //vcl_string name(){return "brecUpdateChangesWrtAreaProcess";}
  vcl_string name(){return "brecBayesianUpdateProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_bayesian_update_process_h_
