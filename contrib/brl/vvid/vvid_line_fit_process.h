// This is brl/vvid/vvid_line_fit_process.h
#ifndef vvid_line_fit_process_h_
#define vvid_line_fit_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_line_fit_process
//   Computes line_fits on each video frame
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy May 16, 2003    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_fit_lines_params.h>
#include <vvid/vvid_video_process.h>

class vvid_line_fit_process : public vvid_video_process, public sdet_fit_lines_params
{
 public:
  vvid_line_fit_process(sdet_fit_lines_params & flp);
  ~vvid_line_fit_process();
  virtual process_data_type get_input_type() const { return TOPOLOGY; }
  virtual process_data_type get_output_type() const { return SPATIAL_OBJECT; }
  //: compute line_fits on a set of input vtol_edges
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vvid_line_fit_process_h_
