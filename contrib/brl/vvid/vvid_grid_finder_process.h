// This is brl/vvid/vvid_grid_finder_process.h
#ifndef vvid_grid_finder_process_h_
#define vvid_grid_finder_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_grid_finder_process
//   Matches a grid to an image, assuming a homography between them
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy May 17, 2003    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_grid_finder_params.h>
#include <vvid/vvid_video_process.h>

class vvid_grid_finder_process : public vvid_video_process, public sdet_detector_params,
                                 public sdet_fit_lines_params, public sdet_grid_finder_params
{
 public:
  vvid_grid_finder_process(sdet_detector_params & dp, sdet_fit_lines_params& flp, sdet_grid_finder_params& gfp);
  ~vvid_grid_finder_process();
  virtual process_data_type get_input_type() const { return IMAGE; }
  virtual process_data_type get_output_type() const { return SPATIAL_OBJECT; }

  //: compute van duc edges, line segments, and then match the grid
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vvid_grid_finder_process_h_
