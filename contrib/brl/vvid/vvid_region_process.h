//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_region_process_h_
#define vvid_region_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_region_process
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
#include <sdet/sdet_detector_params.h>
#include <vvid/vvid_video_process.h>

class vvid_region_process : public vvid_video_process, public sdet_detector_params
{
public:
  vvid_region_process(sdet_detector_params & dp);
  ~vvid_region_process();
  virtual output_type get_output_type(){return TOPOLOGY;}
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish(){return true;}  
private:
  //members

};


#endif // vvid_region_process_h_
