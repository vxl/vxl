//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_edge_process_h_
#define vvid_edge_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_edge_process
//   Computes edges on each video frame
//
// \endbrief
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 11, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <sdet/sdet_detector_params.h>
#include <vvid/vvid_video_process.h>

class vvid_edge_process : public vvid_video_process, public sdet_detector_params
{
public:
  vvid_edge_process(sdet_detector_params & dp);
  ~vvid_edge_process();

  //: compute edges on the input image
  virtual bool execute();

private:
  //members

};


#endif // vvid_edge_process_h_
