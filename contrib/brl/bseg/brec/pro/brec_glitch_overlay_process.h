// This is brl/bseg/brec/pro/brec_glitch_overlay_process.h
#ifndef brec_glitch_overlay_process_h_
#define brec_glitch_overlay_process_h_
//:
// \file
// \brief A class to generate a new probability map that extends glitch detection probability over to its effective region
//
// \author Ozge Can Ozcanli
// \date 12/09/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_glitch_overlay_process : public bprb_process
{
 public:

  brec_glitch_overlay_process();

  //: Copy Constructor (no local data)
  brec_glitch_overlay_process(const brec_glitch_overlay_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_glitch_overlay_process(){};

  //: Clone the process
  virtual brec_glitch_overlay_process* clone() const {return new brec_glitch_overlay_process(*this);}

  vcl_string name(){return "brecGlitchOverlayProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_glitch_overlay_process_h_
