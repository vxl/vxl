//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/vvid/vvid_harris_corner_process.h
#ifndef vvid_harris_corner_process_h_
#define vvid_harris_corner_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief live vvid_harris_corner_process
//   Computes Harris corners on each video frame
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy February 26, 2002    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <sdet/sdet_harris_detector_params.h>
#include <vvid/vvid_video_process.h>

class vvid_harris_corner_process : public vvid_video_process, public sdet_harris_detector_params
{
 public:
  vvid_harris_corner_process(sdet_harris_detector_params & hdp);
  ~vvid_harris_corner_process();
  virtual output_type get_output_type(){return SPATIAL_OBJECT;}

  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish(){return true;}
 private:
  //members
};


#endif // vvid_harris_corner_process_h_
