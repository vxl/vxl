// This is brl/bseg/vpro/vpro_harris_corner_process.h
#ifndef vpro_harris_corner_process_h_
#define vpro_harris_corner_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Computes Harris corners on each video frame
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - February 26, 2002 - Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <sdet/sdet_harris_detector_params.h>
#include <vpro/vpro_video_process.h>

class vpro_harris_corner_process : public vpro_video_process, public sdet_harris_detector_params
{
 public:
  vpro_harris_corner_process(sdet_harris_detector_params & hdp);
  ~vpro_harris_corner_process();
  virtual process_data_type get_output_type() { return SPATIAL_OBJECT; }

  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish() { return true; }
};


#endif // vpro_harris_corner_process_h_
