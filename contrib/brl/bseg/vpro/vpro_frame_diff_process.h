// This is brl/bseg/vpro/vpro_frame_diff_process.h
#ifndef vpro_frame_diff_process_h_
#define vpro_frame_diff_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes frame difference (first output is all zeros)
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - February 16, 2003 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vil1/vil1_memory_image_of.h>
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_frame_diff_params.h>

class vpro_frame_diff_process : public vpro_video_process,
                                public vpro_frame_diff_params
{
 public:
  vpro_frame_diff_process(vpro_frame_diff_params fdp);
  ~vpro_frame_diff_process();
  virtual process_data_type get_output_type() { return IMAGE; }
  //: difference sequential frames
  virtual bool execute();
  virtual bool finish();
 private:
  //members
  vpro_frame_diff_process();
  bool first_frame_;
  vil1_memory_image_of<float> img0_;
};

#endif // vpro_frame_diff_process_h_
