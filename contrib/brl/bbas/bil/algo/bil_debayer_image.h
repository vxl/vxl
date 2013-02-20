#ifndef bil_debayer_image_h_
#define bil_debayer_image_h_
//:
// \file
// \brief Debayer Images.
// \author Vishal Jain


#include <vil/vil_image_view.h>

#include <vcl_vector.h>



class bil_debayer_image
{

 public:

 static void bil_debayer_GRBG(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * outimage);

 static void bil_debayer_BGGR(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * outimage);
  
};

#endif
