// This is brl/bseg/bbgm/pro/bbgm_update_dist_image_stream_process.h
#ifndef bbgm_update_dist_image_stream_process_h_
#define bbgm_update_dist_image_stream_process_h_
//:
// \file
// \brief A process for updating a distribution image
// \author J.L. Mundy
// \date February 9, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vidl2/vidl2_istream_sptr.h>

class bbgm_update_dist_image_stream_process : public bprb_process
{
 public:

  bbgm_update_dist_image_stream_process();

  //: Copy Constructor (no local data)
  bbgm_update_dist_image_stream_process(const bbgm_update_dist_image_stream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bbgm_update_dist_image_stream_process();

  //: Clone the process
  virtual bbgm_update_dist_image_stream_process* clone() const {return new bbgm_update_dist_image_stream_process(*this);}

  vcl_string name(){return "UpdateDistImageProcess";}

  bool init();
  bool execute();
  bool finish(){return true;}

 private:
  vidl2_istream_sptr istream_;
  unsigned ni_;
  unsigned nj_;
};

#endif //bbgm_update_dist_image_stream_process_h_
