// This is brl/bseg/vpro/vpro_lucas_kanade_process.h
#ifndef vpro_lucas_kanade_process_h_
#define vpro_lucas_kanade_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  computes the Lucas-Kanade motion vector.
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - February 19, 2003 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vil1/vil1_memory_image_of.h>
#include <vpro/vpro_video_process.h>

class vpro_lucas_kanade_process : public vpro_video_process
{
 public:
  enum state_symbol {NO_IMAGE=0, FIRST_IMAGE, IN_PROCESS};
  vpro_lucas_kanade_process(bool down_sample=false, int window_size=2,
                            double thresh=20000);
  ~vpro_lucas_kanade_process();
  virtual process_data_type get_output_type() { return IMAGE; }
  //: compute lucas_kanade
  virtual bool execute();
  virtual bool finish();
  bool downsample_;
  int window_size_;
  double thresh_;
 private:
  //local methods
  void compute_lucas_kanade(vil1_memory_image_of<float>& image);
  void update_queue(vil1_image ix);
  //members
  state_symbol state_;
  bool first_frame_;
  vcl_vector<vil1_image> queue_;
};

#endif // vpro_lucas_kanade_process_h_
