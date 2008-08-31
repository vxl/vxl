// This is brl/bpro/core/vil_pro/vil_combine_grey_images_process.h
#ifndef vil_combine_grey_images_process_h_
#define vil_combine_grey_images_process_h_
//:
// \file
// \brief A process for combining grey scale images to multichannel byte images.
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date 8/20/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vcl_vector.h>

//: This process combines grey scale images into a single multi-channel image of the same format as input images.
// All input images must be of the same type.
// Only float images are supported for now.
// Further support should be implemented as needed.

class vil_combine_grey_images_process : public bprb_process
{
 public:

  vil_combine_grey_images_process();

  //: Copy Constructor (no local data)
  vil_combine_grey_images_process(const vil_combine_grey_images_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_combine_grey_images_process();

  //: Clone the process
  virtual vil_combine_grey_images_process* clone() const {return new vil_combine_grey_images_process(*this);}

  vcl_string name(){return "VilCombineGreyImagesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

 protected:
  bool get_images(vcl_string file, vcl_vector<vil_image_view_base_sptr>  &grey_imgs);
  bool combine(vcl_vector<vil_image_view_base_sptr>const  &grey_imgs,
                                         vil_image_view_base_sptr &multi_band_imgs);
  unsigned width_;
  unsigned height_;
};

#endif //vil_combine_grey_images_process_h_
