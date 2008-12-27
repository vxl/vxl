// This is brl/bpro/core/vil_pro/vil_image_pair_process.h
#ifndef vil_image_pair_process_h_
#define vil_image_pair_process_h_
//:
// \file
// \brief A process for combining two images into a single image (side by side)
// \author J.L. Mundy
// \date October 11, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Stretchs an image using a specified functor
//  For color, each channel is independently passed through the functor
class vil_image_pair_process : public bprb_process
{
 public:

  vil_image_pair_process();

  //: Copy Constructor (no local data)
  vil_image_pair_process(const vil_image_pair_process& othe_pr) : bprb_process(*static_cast<const bprb_process*>(&othe_pr)) { }

  ~vil_image_pair_process();

  //: Clone the process
  virtual vil_image_pair_process* clone() const { return new vil_image_pair_process(*this); }

  vcl_string name() { return "ImagePairProcess"; }

  bool init() { return true; }
  bool execute();//images have to be the same size otherwise fails
  bool finish() { return true; }
};

#endif // vil_image_pair_process_h_
