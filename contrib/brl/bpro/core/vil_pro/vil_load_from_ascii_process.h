// This is brl/bpro/core/vil_pro/vil_load_from_ascii_process.h
#ifndef vil_load_from_ascii_process_h_
#define vil_load_from_ascii_process_h_
//:
// \file
// \brief A process for creating images from ascii file
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date 8/27/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vcl_vector.h>


//: This procces reads acii file an creates a vil_image_view of
//  the data. Only float images are supported. Other formats 
//  we'll become supported as needed
class vil_load_from_ascii_process : public bprb_process
{
 public:

  vil_load_from_ascii_process();

  //: Copy Constructor (no local data)
  vil_load_from_ascii_process(const vil_load_from_ascii_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_load_from_ascii_process();

  //: Clone the process
  virtual vil_load_from_ascii_process* clone() const {return new vil_load_from_ascii_process(*this);}

  vcl_string name(){return "LoadFromAsciiProcess";}
 
  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

protected:
  
};

#endif //vil_load_from_ascii_process_h_
