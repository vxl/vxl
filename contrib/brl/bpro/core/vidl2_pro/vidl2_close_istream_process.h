// This is brl/bpro/core/vidl2_pro/vidl2_close_istream_process.h
#ifndef vidl2_close_istream_process_h_
#define vidl2_close_istream_process_h_
//:
// \file
// \brief A process for closing a vidl2_istream in the database
//        The python script tends to hang on indefinitely if the stream is not closed properly
//
// \author O. C. Ozcanli
// \date 12/18/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class vidl2_close_istream_process : public bprb_process
{
 public:

  vidl2_close_istream_process();

  //: Copy Constructor (no local data)
  vidl2_close_istream_process(const vidl2_close_istream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vidl2_close_istream_process();

  //: Clone the process
  virtual vidl2_close_istream_process* clone() const {return new vidl2_close_istream_process(*this);}

  vcl_string name(){return "CloseVidl2IstreamProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vidl2_close_istream_process_h_
