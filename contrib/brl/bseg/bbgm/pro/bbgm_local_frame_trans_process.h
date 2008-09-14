// This is brl/bseg/bbgm/pro/bbgm_local_frame_trans_process.h
#ifndef bbgm_local_frame_trans_process_h_
#define bbgm_local_frame_trans_process_h_
//:
// \file
// \brief A process for computing a local frame translation based on maximizing appearance probability
// \author J.L. Mundy
// \date August 25, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vidl2/vidl2_istream_sptr.h>

class bbgm_local_frame_trans_process : public bprb_process
{
 public:

  bbgm_local_frame_trans_process();

  //: Copy Constructor (no local data)
  bbgm_local_frame_trans_process(const bbgm_local_frame_trans_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bbgm_local_frame_trans_process();

  //: Clone the process
  virtual bbgm_local_frame_trans_process* clone() const {return new bbgm_local_frame_trans_process(*this);}

  vcl_string name(){return "LocalFrameTransProcess";}

  bool init(){return true;}
  bool execute();
  bool finish(){return true;}

 private:
};

#endif //bbgm_local_frame_trans_process_h_
