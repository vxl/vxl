// This is brl/bseg/vpro/vpro_line_fit_process.h
#ifndef vpro_line_fit_process_h_
#define vpro_line_fit_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vpro_line_fit_process
// \author J.L. Mundy
//
//   Computes line_fits on each video frame
//
// \verbatim
//  Modifications
//   J.L. Mundy - May 16, 2003 - Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_fit_lines_params.h>
#include <vpro/vpro_video_process.h>

class vpro_line_fit_process : public vpro_video_process, public sdet_fit_lines_params
{
 public:
  vpro_line_fit_process(sdet_fit_lines_params & flp);
 ~vpro_line_fit_process();
  virtual process_data_type get_input_type() { return TOPOLOGY; }
  virtual process_data_type get_output_type() { return SPATIAL_OBJECT; }
  //: compute line_fits on a set of input vtol_edges
  virtual bool execute();
  virtual bool finish() { return true; }
};


#endif // vpro_line_fit_process_h_
