// This is brl/vvid/vvid_epipolar_space_process.h
#ifndef vvid_epipolar_space_process_h_
#define vvid_epipolar_space_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_epipolar_space_process
//
//  An epipolar_space visualization by mapping grey levels from a stereo
//  pair of images to color space. Simply create a green-blue image from
//  the two input grey levels
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 11, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vvid/vvid_video_process.h>

class vvid_epipolar_space_process : public vvid_video_process
{
 public:
  vvid_epipolar_space_process();
  ~vvid_epipolar_space_process();
  virtual output_type get_output_type(){return IMAGE;}
  //: We assume that two grey scale images are in the input list
  virtual bool execute();
  virtual bool finish(){return true;}
};

#endif // vvid_epipolar_space_process_h_
