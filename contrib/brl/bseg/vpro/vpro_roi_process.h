// This is brl/bseg/vpro/vpro_roi_process.h
#ifndef vpro_roi_process_h_
#define vpro_roi_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Creates a video with a roi cut out
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
#include <vpro/vpro_vil_video_process.h>

class vpro_roi_process : public vpro_vil_video_process
{
 public:
  vpro_roi_process(vcl_string const & video_file, 
                   const unsigned x0,
                   const unsigned y0,
                   const unsigned xsize,
                   const unsigned ysize);
  ~vpro_roi_process();
  virtual process_data_type get_output_type() { return IMAGE; }
  virtual bool execute();
  virtual bool finish();
 private:
  vpro_roi_process();
  //members
  unsigned x0_;
  unsigned y0_;
  unsigned xsize_;
  unsigned ysize_;
  vcl_string video_file_;//output file name
  vcl_list<vil_image_resource_sptr> out_frames_;
};

#endif // vpro_roi_process_h_
