// This is brl/bseg/strk/strk_snippet_extractor_process.h
#ifndef strk_snippet_extractor_process_h_
#define strk_snippet_extractor_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Extracts a set of snippets around the tracked vehicle
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 5, 2004    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_string.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vil1/vil1_image.h>
#include <brip/brip_roi_sptr.h>
#include <vpro/vpro_video_process.h>

class strk_snippet_extractor_process : public vpro_video_process
{
 public:
  strk_snippet_extractor_process();
  ~strk_snippet_extractor_process();
  virtual process_data_type get_output_type() { return IMAGE; }

  //: track to next frame
  virtual bool execute();
  //: output the snippets
  virtual bool finish();
  //: track input filename
  bool set_input_file(vcl_string const& track_file_name);
  //: snippet directory
  void set_snippet_directory(vcl_string const& snip_dir);
  //: fractional margin for snippet roi around the tracked object
  void set_margin_fraction(const double margin_fraction){margin_frac_=margin_fraction;}
 private:
  //local functions
  bool extract_rois(vcl_vector<vtol_face_2d_sptr> const & tracked_faces);
  //members
  bool failure_;
  bool first_frame_;
  vcl_string track_file_;
  vcl_string snippet_directory_;
  double margin_frac_;
  vcl_vector<brip_roi_sptr> tracked_rois_;
  vcl_vector<vil1_image> tracked_snippets_;
};


#endif // strk_snippet_extractor_process_h_
