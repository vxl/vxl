#ifndef bil_debayer_image_h_
#define bil_debayer_image_h_
//:
// \file
// \brief Debayer Images.
// \author Vishal Jain


#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif



class bil_debayer_image
{

 public:

 static void bil_debayer_GRBG(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * outimage);

 static void bil_debayer_BGGR(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * outimage);

};

#endif
