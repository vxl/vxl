// This is mul/vimt3d/tests/test_crop.cxx

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_crop.h>
#include <testlib/testlib_test.h>


static void test_crop()
{
  float dx = 13.0;
  float dy = 13.0;
  float dz = 11.0;
  int idx = unsigned(dx);
  int idy = unsigned(dy);
  int idz = unsigned(dz);
  vimt3d_image_3d_of<float> original_image(idx,idy,idz);
  vimt3d_transform_3d w2im;
  w2im.set_zoom_only(0.3,1.1,0.7,-3,57,23);
  original_image.set_world2im(w2im);

  // create image
  float count=0;
  for (float & it : original_image.image())
  {
     it = count;
     ++count;
  }

  std::vector<double> lo, hi;
  original_image.world_bounds(lo,hi);

  // crop whole image
  {
    // make bigger than image to test bound checking
    vgl_box_3d<double> box(vgl_point_3d<double>(lo[0]-2.0,lo[1]-1.1,lo[2]-9.9),
                           vgl_point_3d<double>(hi[0]+2.2,hi[1]+7.8,hi[2]+9.9));

    vimt3d_image_3d_of<float> cropped_image=vimt3d_crop(original_image,box);

    float diff=0;
    for (unsigned k=0;k<original_image.image().nk();++k)
      for (unsigned j=0;j<original_image.image().nj();++j)
        for (unsigned i=0;i<original_image.image().ni();++i)
        {
          diff += (cropped_image.image()(i,j,k) - original_image.image()(i,j,k));
        }
        TEST_NEAR("WHole image crop Difference between images is 0",diff,0,1e-6);
  }

   // crop part of image
  {
    // make bigger than image to test bound checking
    double d=0.001; // to avoid ceil/flooe effects close to integral value
    vgl_box_3d<double> box(w2im.inverse()(vgl_point_3d<double>(1+d,2+d,3+d)),
                           w2im.inverse()(vgl_point_3d<double>(2-d,4-d,7-d)) );

    vimt3d_image_3d_of<float> cropped_image=vimt3d_crop(original_image,box);

    float diff=0;
    for (unsigned k=3;k<=7;++k)
      for (unsigned j=2;j<=4;++j)
        for (unsigned i=1;i<=2;++i)
        {
          diff += (cropped_image.image()(i-1,j-2,k-3) - original_image.image()(i,j,k));
        }
        TEST_NEAR("Part image crop  Difference between images is 0",diff,0,1e-6);
  }
}

TESTMAIN(test_crop);
