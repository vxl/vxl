//-*-c++-*-
// This is brl/vpro/vpro_track_display_process.h
#ifndef vpro_track_display_process_h_
#define vpro_track_display_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Displays a tracked polygon on a video
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 14, 2003    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <sdet/sdet_tracker_params.h>
#include <sdet/sdet_tracker.h>
#include <vpro/vpro_video_process.h>

class vpro_track_display_process : public vpro_video_process
{
 public:
  vpro_track_display_process();
  ~vpro_track_display_process();
  virtual process_data_type get_output_type(){return TOPOLOGY;}

  //: track to next frame
  virtual bool execute();
  virtual bool finish(){return true;}
  bool set_input_file(vcl_string const& file_name);
 private:
  bool input_tracked_faces(vcl_ifstream & str);
  //members
  bool failure_;
  bool first_frame_;
  vcl_string track_file_;
  int start_frame_;
  vcl_vector<vtol_face_2d_sptr> tracked_faces_;
};


#endif // vpro_track_display_process_h_
