// This is brl/bseg/vpro/vpro_grid_finder_process.h
#ifndef vpro_grid_finder_process_h_
#define vpro_grid_finder_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vpro_grid_finder_process
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
#include <vpro/vpro_video_process.h>
#include <vcl_string.h>

class vpro_grid_finder_process : public vpro_video_process,
                                 public sdet_detector_params,
                                 public sdet_fit_lines_params,
                                 public sdet_grid_finder_params
{
 public:
  vpro_grid_finder_process(sdet_detector_params & dp, sdet_fit_lines_params& flp, sdet_grid_finder_params& gfp);
  ~vpro_grid_finder_process();
  void set_output_file(vcl_string filename);
  virtual process_data_type get_input_type(){return IMAGE;}
  virtual process_data_type get_output_type(){return SPATIAL_OBJECT;}

  //: compute van duc edges, line segments, and then match the grid
  virtual bool execute();
  virtual bool finish(){return true;}
  vcl_vector<float> frame_scores_;
 private:
  vcl_string output_filename_;
};

#endif // vpro_grid_finder_process_h_
