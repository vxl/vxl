// This is core/vil/tests/test_deep_copy_3_plane.cxx
#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

bool equal_image(const vil_image_view< vxl_byte >& im0,
                 const vil_image_view< vxl_byte >& im1)
{
  if (im0.ni() != im1.ni() || im0.nj()!=im1.nj() || im0.nplanes() != im1.nplanes()) return false;
  for (int i=0;i<4;++i)
   for (int j=0;j<4;++j)
    for (int p=0;p<3;++p)
    {
      if (im0(i,j,p) != im1(i,j,p)) return false;
    }
    return true;
}

static void test_deep_copy_3_plane()
{
  vxl_byte dummy(1);

  vil_image_view< vxl_byte >  rgbrgb_im=vil_new_image_view_j_i_plane (4,4,3, dummy);

  vil_image_view< vxl_byte >  rrggbb_im(4,4,3);
  for (int i=0;i<4;++i)
   for (int j=0;j<4;++j)
    for (int p=0;p<3;++p)
    {
      rgbrgb_im(i,j,p)=1+4*j+16*p;
      rrggbb_im(i,j,p)=1+4*j+16*p;
     }

  vil_image_view< vxl_byte >  rgbrgb_im_new;
  vil_image_view< vxl_byte >  rrggbb_im_new;

  rgbrgb_im_new.deep_copy(rgbrgb_im);
  rrggbb_im_new.deep_copy(rrggbb_im);

  TEST("Deep Copy rgbrgb",equal_image(rgbrgb_im,rgbrgb_im_new),true);
  TEST("Deep Copy rrggbb",equal_image(rrggbb_im,rrggbb_im_new),true);
}

TESTMAIN(test_deep_copy_3_plane);
