// This is brl/bpro/core/vil_pro/vil_gradient_process.h
#ifndef vil_gradient_process_h_
#define vil_gradient_process_h_
//:
// \file
// \brief A process for computing the gradient of an image. Returns vector and magnitude
// \author J.L. Mundy
// \date August 25, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Maps an image using a specified functor
//  For color, each channel is independently passed through the functor
class vil_gradient_process : public bprb_process
{
 public:

  vil_gradient_process();

  //: Copy Constructor (no local data)
  vil_gradient_process(const vil_gradient_process& othe_pr) : bprb_process(*static_cast<const bprb_process*>(&othe_pr)) { }

  ~vil_gradient_process();

  //: Clone the process
  virtual vil_gradient_process* clone() const { return new vil_gradient_process(*this); }

  vcl_string name() { return "VilGradientProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // vil_gradient_process_h_
