// This is brl/bseg/vpro/vpro_edge_line_process.h
#ifndef vpro_edge_line_process_h_
#define vpro_edge_line_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vpro_edge_line_process
// \author J.L. Mundy
//
//   Computes vd edges and then line segments on each video frame
//
// \verbatim
//  Modifications
//   J.L. Mundy - May 16, 2003 - Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_lines_params.h>
#include <vpro/vpro_video_process.h>

class vpro_edge_line_process : public vpro_video_process, public sdet_detector_params, public sdet_fit_lines_params
{
 public:
  vpro_edge_line_process(sdet_detector_params & dp, sdet_fit_lines_params& flp);
 ~vpro_edge_line_process();
  virtual process_data_type get_input_type() { return IMAGE; }
  virtual process_data_type get_output_type() { return SPATIAL_OBJECT; }

  //: compute van duc edges and then line segments
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vpro_edge_line_process_h_
