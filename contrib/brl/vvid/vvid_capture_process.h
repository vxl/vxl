// This is brl/vvid/vvid_capture_process.h
#ifndef vvid_capture_process_h_
#define vvid_capture_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Captures a vidl movie from a live video feed
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy February 13, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_list.h>
#include <vcl_string.h>
#include <vvid/vvid_video_process.h>

class vvid_capture_process : public vvid_video_process
{
 public:
  vvid_capture_process(vcl_string & video_file);
  ~vvid_capture_process();
  virtual process_data_type get_output_type() const { return NOTYPE; }
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish();
 private:
  vvid_capture_process();
  //members
  vcl_string video_file_;
  vcl_list<vil_image> frames_;
};

#endif // vvid_capture_process_h_
