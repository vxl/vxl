// This is brl/bseg/strk/strk_info_tracker_process.h
#ifndef strk_info_tracker_process_h_
#define strk_info_tracker_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief tracks a correlation window across frames
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - August 21, 2002 - Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vtol/vtol_face_2d_sptr.h>
#include <strk/strk_info_tracker_params.h>
#include <strk/strk_info_tracker.h>
#include <vpro/vpro_video_process.h>

class strk_info_tracker_process : public vpro_video_process
{
 public:
  strk_info_tracker_process(strk_info_tracker_params & tp);
  ~strk_info_tracker_process();
  virtual process_data_type get_output_type() { return TOPOLOGY; }

  //: track to next frame
  virtual bool execute();
  virtual bool finish();
  bool set_track_output_file(vcl_string const& file_name);
  bool set_hist_output_file(vcl_string const& file_name);

  //: special methods
  // for display
  vcl_vector<unsigned char> color_index(){return color_index_;}
 private:
  //members
  bool write_tracked_faces_;
  bool write_tracked_hist_;
  vcl_string track_file_;
  vcl_string hist_file_;
  bool failure_;
  bool first_frame_;
  strk_info_tracker tracker_;
  int n_verts_;
  int start_frame_;
  vcl_vector<vtol_face_2d_sptr> tracked_faces_;
  vcl_vector<vcl_vector<float> > tracked_hist_;
  vcl_vector<unsigned char> color_index_;
};


#endif // strk_info_tracker_process_h_
