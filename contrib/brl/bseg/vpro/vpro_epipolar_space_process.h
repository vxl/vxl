// This is brl/bseg/vpro/vpro_epipolar_space_process.h
#ifndef vpro_epipolar_space_process_h_
#define vpro_epipolar_space_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vpro_epipolar_space_process
// \author J.L. Mundy
//
//  An epipolar_space visualization by mapping grey levels from a stereo
//  pair of images to color space. Simply create a green-blue image from
//  the two input grey levels
//
// \verbatim
//  Modifications
//   J.L. Mundy - October 11, 2002 - Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vpro/vpro_video_process.h>

class vpro_epipolar_space_process : public vpro_video_process
{
 public:
  vpro_epipolar_space_process();
  ~vpro_epipolar_space_process();
  virtual process_data_type get_output_type() { return IMAGE; }
  //: We assume that two grey scale images are in the input list
  virtual bool execute();
  virtual bool finish() { return true; }
};

#endif // vpro_epipolar_space_process_h_
