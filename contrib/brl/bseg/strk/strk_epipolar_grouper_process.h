// This is brl/bseg/strk/strk_epipolar_grouper_process.h
#ifndef strk_epipolar_grouper_process_h_
#define strk_epipolar_grouper_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief computes epipolar matches for tracking curves in video
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 26, 2003    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <sdet/sdet_detector_params.h>
#include <strk/strk_epipolar_grouper_params.h>
#include <strk/strk_epipolar_grouper.h>
#include <vpro/vpro_video_process.h>

class strk_epipolar_grouper_process : public vpro_video_process, public sdet_detector_params, public strk_epipolar_grouper_params

{
 public:
  strk_epipolar_grouper_process(sdet_detector_params& dp, 
                                strk_epipolar_grouper_params & tp);
  ~strk_epipolar_grouper_process();

  virtual process_data_type get_input_type(){return IMAGE;}
  virtual process_data_type get_output_type(){return SPATIAL_OBJECT;}

  //: track to next frame
  virtual bool execute();
  virtual bool finish(){return true;}
  void set_epi_line(const double pu, const double pv);
 private:
  //members
  bool failure_;
  bool first_frame_;
  strk_epipolar_grouper epipolar_grouper_;
};

#endif // strk_epipolar_grouper_process_h_
