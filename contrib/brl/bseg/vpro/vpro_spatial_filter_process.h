//this-sets-emacs-to-*-c++-*-mode
#ifndef vpro_spatial_filter_process_h_
#define vpro_spatial_filter_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes frame difference (first output is all zeros)
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 28, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vil1/vil1_memory_image_of.h>
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_spatial_filter_params.h>

class vpro_spatial_filter_process : public vpro_video_process, public vpro_spatial_filter_params
{
 public:
  vpro_spatial_filter_process(vpro_spatial_filter_params vfp);
  ~vpro_spatial_filter_process();
  virtual process_data_type get_output_type(){return IMAGE;}
  //: difference sequential frames
  virtual bool execute();
  virtual bool finish();
 private:
  //members
  vpro_spatial_filter_process();
};

#endif // vpro_spatial_filter_process_h_
