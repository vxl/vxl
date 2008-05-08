// This is brl/bpro/core/vil_pro/vil_rgbi_to_grey_process.h
#ifndef vil_rgbi_to_grey_process_h_
#define vil_rgbi_to_grey_process_h_
//:
// \file
// \brief A process for converting rgbi to grey image.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class vil_rgbi_to_grey_process : public bprb_process
{
 public:

  vil_rgbi_to_grey_process();

  //: Copy Constructor (no local data)
  vil_rgbi_to_grey_process(const vil_rgbi_to_grey_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_rgbi_to_grey_process();

  //: Clone the process
  virtual vil_rgbi_to_grey_process* clone() const {return new vil_rgbi_to_grey_process(*this);}

  vcl_string name(){return "RGBItoGreyProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vil_rgbi_to_grey_process_h_
