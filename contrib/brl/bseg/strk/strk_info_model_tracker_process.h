// This is brl/bseg/strk/strk_info_model_tracker_process.h
#ifndef strk_info_model_tracker_process_h_
#define strk_info_model_tracker_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief tracks an articulated model using sdet_info_model_tracker
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 06, 2003    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <strk/strk_info_model_tracker_params.h>
#include <strk/strk_info_model_tracker.h>
#include <vpro/vpro_video_process.h>

class strk_info_model_tracker_process : public vpro_video_process
{
 public:
  strk_info_model_tracker_process(strk_info_model_tracker_params & tp);
  ~strk_info_model_tracker_process();
  virtual process_data_type get_output_type(){return TOPOLOGY;}

  virtual bool execute();
  virtual bool finish();
  bool set_output_file(vcl_string const& file_name);
 private:
  //members
  bool write_tracked_models_;
  vcl_string track_file_;
  bool failure_;
  bool first_frame_;
  int start_frame_;
  strk_info_model_tracker model_tracker_;
  vcl_vector<strk_art_info_model_sptr> tracked_models_;
};


#endif // strk_info_model_tracker_process_h_
