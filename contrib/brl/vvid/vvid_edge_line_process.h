// This is brl/vvid/vvid_edge_line_process.h
#ifndef vvid_edge_line_process_h_
#define vvid_edge_line_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_edge_line_process
//   Computes vd edges and then line segments on each video frame
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy May 16, 2003    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_lines_params.h>
#include <vvid/vvid_video_process.h>

class vvid_edge_line_process : public vvid_video_process, public sdet_detector_params, public sdet_fit_lines_params
{
 public:
  vvid_edge_line_process(sdet_detector_params & dp, sdet_fit_lines_params& flp);
  ~vvid_edge_line_process();
  virtual process_data_type get_input_type() const { return IMAGE; }
  virtual process_data_type get_output_type() const { return SPATIAL_OBJECT; }

  //: compute van duc edges and then line segments
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vvid_edge_line_process_h_
