// This is brl/bseg/vpro/vpro_spatial_filter_process.h
#ifndef vpro_spatial_filter_process_h_
#define vpro_spatial_filter_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Filter an image with a Gaussian blocking filter
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 28, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_spatial_filter_params.h>

class vpro_spatial_filter_process : public vpro_video_process, public vpro_spatial_filter_params
{
  // default constructor is private
  vpro_spatial_filter_process() {}
 public:
  vpro_spatial_filter_process(vpro_spatial_filter_params vfp) : vpro_spatial_filter_params(vfp) {}
 ~vpro_spatial_filter_process() {}
  virtual process_data_type get_output_type() { return IMAGE; }
  //: difference sequential frames
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vpro_spatial_filter_process_h_
