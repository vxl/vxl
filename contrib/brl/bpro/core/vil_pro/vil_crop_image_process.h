// This is brl/bpro/core/vil_pro/vil_crop_image_process.h
#ifndef vil_crop_image_process_h_
#define vil_crop_image_process_h_
//:
// \file
// \brief A process for cropping an image
// \author Ozge C. Ozcanli
// \date 12/30/08
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Stretchs an image using a specified functor
//  For color, each channel is independently passed through the functor
class vil_crop_image_process : public bprb_process
{
 public:

  vil_crop_image_process();

  //: Copy Constructor (no local data)
  vil_crop_image_process(const vil_crop_image_process& othe_pr) : bprb_process(*static_cast<const bprb_process*>(&othe_pr)) { }

  ~vil_crop_image_process();

  //: Clone the process
  virtual vil_crop_image_process* clone() const { return new vil_crop_image_process(*this); }

  vcl_string name() { return "CropImageProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // vil_crop_image_process_h_
