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
// \endverbatim
//--------------------------------------------------------------------------------
#include <bdgl/bdgl_curve_tracker.h>
#include <vpro/vpro_video_process.h>
#include <sdet/sdet_detector_params.h>

class vpro_curve_tracking_process : public vpro_video_process, public bdgl_curve_tracker
{
 public:
  vpro_curve_tracking_process(const bdgl_curve_tracker_params & tp, const sdet_detector_params & dp);
  ~vpro_curve_tracking_process();

  virtual process_data_type get_output_type(){return SPATIAL_OBJECT;}
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish(){return true;}

 private:
  //members
  sdet_detector_params detect_params_;
};

#endif // vpro_curve_tracking_process_h_
