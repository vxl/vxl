// This is brl/bseg/vpro/vpro_capture_process.h
#ifndef vpro_capture_process_h_
#define vpro_capture_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Captures a vidl movie from a live video feed
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - February 13, 2003 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_list.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vpro/vpro_video_process.h>

class vpro_capture_process : public vpro_video_process
{
 public:
  vpro_capture_process(vcl_string const & video_file);
  ~vpro_capture_process();
  virtual process_data_type get_output_type() { return NOTYPE; }
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish();
 private:
  vpro_capture_process();
  bool save_time_stamps();
  //members
  static vul_timer time_;
  static bool capture_started_;
  vcl_string video_file_;
  vcl_list<vil1_image> frames_;
  vcl_vector<long> time_stamps_;
};

#endif // vpro_capture_process_h_
