// This is brl/vpro/vpro_info_tracker_process.h
#ifndef vpro_info_tracker_process_h_
#define vpro_info_tracker_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief tracks a correlation window across frames
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy August 21, 2002    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_fstream.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <sdet/sdet_info_tracker_params.h>
#include <sdet/sdet_info_tracker.h>
#include <vpro/vpro_video_process.h>

class vpro_info_tracker_process : public vpro_video_process
{
 public:
  vpro_info_tracker_process(sdet_info_tracker_params & tp);
  ~vpro_info_tracker_process();
  virtual process_data_type get_output_type(){return TOPOLOGY;}

  //: track to next frame
  virtual bool execute();
  virtual bool finish();
  bool set_output_file(vcl_string const& file_name);
 private:
  //members
  bool write_tracked_faces_;
  vcl_string track_file_;
  bool failure_;
  bool first_frame_;
  sdet_info_tracker tracker_;
  int n_verts_;
  int start_frame_;
  vcl_vector<vtol_face_2d_sptr> tracked_faces_;
};


#endif // vpro_info_tracker_process_h_
