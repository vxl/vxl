// This is brl/bpro/core/vil_pro/vil_map_image_binary_process.h
#ifndef vil_map_image_binary_process_h_
#define vil_map_image_binary_process_h_
//:
// \file
// \brief A process for mapping an image with a binary functor
// \author Ozge C. Ozcanli
// \date November 13, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Maps an image using a specified functor
//  For color, each channel is independently passed through the functor
class vil_map_image_binary_process : public bprb_process
{
 public:

  vil_map_image_binary_process();

  //: Copy Constructor (no local data)
  vil_map_image_binary_process(const vil_map_image_binary_process& othe_pr) : bprb_process(*static_cast<const bprb_process*>(&othe_pr)) { }

  ~vil_map_image_binary_process();

  //: Clone the process
  virtual vil_map_image_binary_process* clone() const { return new vil_map_image_binary_process(*this); }

  vcl_string name() { return "MapImageBinaryProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // vil_map_image_binary_process_h_
