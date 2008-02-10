// This is brl/bseg/bbgm/pro/bbgm_load_image_of_process.h
#ifndef bbgm_load_image_of_process_h_
#define bbgm_load_image_of_process_h_

//:
// \file
// \brief A process for saving a background image of distributions
// \author J.L. Mundy
// \date February 8, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bbgm_load_image_of_process : public bprb_process
{
 public:
  
  bbgm_load_image_of_process();

  //: Copy Constructor (no local data)
  bbgm_load_image_of_process(const bbgm_load_image_of_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bbgm_load_image_of_process();

  //: Clone the process
  virtual bbgm_load_image_of_process* clone() const {return new bbgm_load_image_of_process(*this);}

  vcl_string name(){return "LoadImageOfProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bbgm_load_image_of_process_h_
