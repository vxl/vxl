// This is brl/bpro/core/vidl2_pro/vidl2_open_dshow_istream_process.h
#ifndef vidl2_open_dshow_istream_process_h_
#define vidl2_open_dshow_istream_process_h_
//:
// \file
// \brief A process for loading a vidl2_istream using DShow library - for windows machine - 
//        Install the necessary codecs so that the stream can be opened with Windows Media Player
//        If so the stream can be opened with this process
//
// \author Ozge C. Ozcanli
// \date 12/18/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class vidl2_open_dshow_istream_process : public bprb_process
{
 public:

  vidl2_open_dshow_istream_process();

  //: Copy Constructor (no local data)
  vidl2_open_dshow_istream_process(const vidl2_open_dshow_istream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vidl2_open_dshow_istream_process();

  //: Clone the process
  virtual vidl2_open_dshow_istream_process* clone() const {return new vidl2_open_dshow_istream_process(*this);}

  vcl_string name(){return "OpenDShowVidl2IstreamProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vidl2_open_dshow_istream_process_h_
