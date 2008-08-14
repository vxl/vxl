// This is brl/bpro/core/vil_pro/vil_convert_to_n_planes_process.h
#ifndef vil_convert_to_n_planes_process_h_
#define vil_convert_to_n_planes_process_h_
//:
// \file
// \brief A process for an image source to a n-planes image.
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date 8/7/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: This process is based on vil_convert_to_n_planes function, refer to this function for limitations.
// It creates an n plane image from any image src.
// If the input images have too many planes, the higher planes will be
// truncated. If the input image has too few planes, the new planes will be
// copies of the first plane.
//
// The pixel type of the output image is the same as the input image.
// However, this function works on scalar pixel types only

class vil_convert_to_n_planes_process : public bprb_process
{
 public:

  vil_convert_to_n_planes_process();

  //: Copy Constructor (no local data)
  vil_convert_to_n_planes_process(const vil_convert_to_n_planes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_convert_to_n_planes_process();

  //: Clone the process
  virtual vil_convert_to_n_planes_process* clone() const {return new vil_convert_to_n_planes_process(*this);}

  vcl_string name(){return "VilConvertToNPlanesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vil_convert_to_n_planes_process_h_
