// This is brl/bseg/vpro/vpro_edge_process.h
#ifndef vpro_edge_process_h_
#define vpro_edge_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes edges on each video frame
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - October 11, 2002 - Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <sdet/sdet_detector_params.h>
#include <vpro/vpro_video_process.h>

class vpro_edge_process : public vpro_video_process, public sdet_detector_params
{
 public:
  vpro_edge_process(sdet_detector_params & dp);
  ~vpro_edge_process();
  virtual process_data_type get_output_type() { return TOPOLOGY; }

  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish() { return true; }
};


#endif // vpro_edge_process_h_
