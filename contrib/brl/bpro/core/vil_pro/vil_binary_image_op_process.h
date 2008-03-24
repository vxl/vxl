// This is brl/bpro/core/vil_pro/vil_binary_image_op_process.h
#ifndef vil_binary_image_op_process_h_
#define vil_binary_image_op_process_h_
//:
// \file
// \brief A process for computing the product of two float images
// \author J.L. Mundy
// \date February 26, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class vil_binary_image_op_process : public bprb_process
{
 public:

  vil_binary_image_op_process();

  //: Copy Constructor (no local data)
  vil_binary_image_op_process(const vil_binary_image_op_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_binary_image_op_process();

  //: Clone the process
  virtual vil_binary_image_op_process* clone() const {return new vil_binary_image_op_process(*this);}

  vcl_string name(){return "ImageProductProcess";}

  bool init(){return true;}
  bool execute();
  bool finish(){return true;}
};

#endif //vil_binary_image_op_process_h_
