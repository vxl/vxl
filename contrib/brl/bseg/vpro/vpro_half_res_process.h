// This is brl/bseg/vpro/vpro_capture_process.h
#ifndef vpro_capture_process_h_
#define vpro_capture_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Downsamples a video to half resolution
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - Sept 24, 2004 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_list.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vpro/vpro_video_process.h>

class vpro_half_res_process : public vpro_video_process
{
 public:
  vpro_half_res_process(vcl_string const & video_file);
  ~vpro_half_res_process();
  virtual process_data_type get_output_type() { return NOTYPE; }
  virtual bool execute();
  virtual bool finish();
 private:
  vpro_half_res_process();
  //members
  vcl_string video_file_;//output file name
  vcl_list<vil1_image> out_frames_;
};

#endif // vpro_half_res_process_h_
