// This is brl/vpro/vpro_curve_tracking_process.h
#ifndef vpro_curve_tracking_process_h_
#define vpro_curve_tracking_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes edge tracking on each video frame
// \author
//   P.L. Bazin
//
// \verbatim
//  Modifications:
//   P.L. Bazin  March 2003  Initial version.
//   Vishal Jain August 2003 added write_to_file function
// \endverbatim
//--------------------------------------------------------------------------------
#include <bdgl/bdgl_curve_tracking.h>
#include <vpro/vpro_video_process.h>
#include <sdet/sdet_detector_params.h>

class vpro_curve_tracking_process : public vpro_video_process ,public bdgl_curve_tracking 
{
 public:
  vpro_curve_tracking_process(bdgl_curve_tracking_params & tp, const sdet_detector_params & dp);
  ~vpro_curve_tracking_process();
 
  virtual process_data_type get_output_type(){return SPATIAL_OBJECT;}
  // for geting the tracks
  bool write_to_file();
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish(){return true;}
  
 private:
  //members
  sdet_detector_params detect_params_;
  bdgl_curve_tracking_params params_;
};

#endif // vpro_curve_tracking_process_h_
