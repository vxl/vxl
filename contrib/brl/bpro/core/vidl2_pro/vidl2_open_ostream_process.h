// This is brl/bpro/core/vil_pro/vidl2_open_ostream_process.h
#ifndef vidl2_open_ostream_process_h_
#define vidl2_open_ostream_process_h_

//:
// \file
// \brief A process for opening a vidl2_ostream - only for image_list_ostream
// \author J.L. Mundy
// \date February 7, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Opens a vidl2_ostream, creating a directory if necessary
//  Inputs:  Index          Type                Purpose
//             0         vcl_string    Directory for the image list
//             1            bool       If true then create the directory
//                                      if it doesn't exist
//
class vidl2_open_ostream_process : public bprb_process
{
 public:
  
  vidl2_open_ostream_process();

  //: Copy Constructor (no local data)
  vidl2_open_ostream_process(const vidl2_open_ostream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vidl2_open_ostream_process();

  //: Clone the process
  virtual vidl2_open_ostream_process* clone() const {return new vidl2_open_ostream_process(*this);}

  vcl_string name(){return "OpenVidl2OstreamProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //vidl2_open_ostream_process_h_
