// This is brl/bpro/core/vil_pro/vidl2_open_istream_process.h
#ifndef vidl2_open_istream_process_h_
#define vidl2_open_istream_process_h_

//:
// \file
// \brief A process for loading a vidl2_istream
// \author J.L. Mundy
// \date February 7, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>
//: Loads an image list video istream
//  Inputs:  Index          Type                Purpose
//             0         vcl_string    Directory for the image list
//

class vidl2_open_istream_process : public bprb_process
{
 public:
  
  vidl2_open_istream_process();

  //: Copy Constructor (no local data)
  vidl2_open_istream_process(const vidl2_open_istream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vidl2_open_istream_process();

  //: Clone the process
  virtual vidl2_open_istream_process* clone() const {return new vidl2_open_istream_process(*this);}

  vcl_string name(){return "LoadVidl2IstreamProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //vidl2_open_istream_process_h_
