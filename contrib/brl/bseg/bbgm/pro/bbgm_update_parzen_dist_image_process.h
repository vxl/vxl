// This is brl/bseg/bbgm/pro/bbgm_update_parzen_dist_image_process.h
#ifndef bbgm_update_parzen_dist_image_process_h_
#define bbgm_update_parzen_dist_image_process_h_
//:
// \file
// \brief A process for updating a parzen distribution image
// \author J.L. Mundy
// \date October 13, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bbgm_update_parzen_dist_image_process : public bprb_process
{
 public:

  bbgm_update_parzen_dist_image_process();

  //: Copy Constructor (no local data)
  bbgm_update_parzen_dist_image_process(const bbgm_update_parzen_dist_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bbgm_update_parzen_dist_image_process();

  //: Clone the process
  virtual bbgm_update_parzen_dist_image_process* clone() const {return new bbgm_update_parzen_dist_image_process(*this);}

  vcl_string name(){return "UpdateParzenDistImageProcess";}

  bool init();
  bool execute();
  bool finish(){return true;}
};

#endif //bbgm_update_parzen_dist_image_process_h_
