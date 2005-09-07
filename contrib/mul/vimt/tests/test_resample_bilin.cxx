// This is mul/vimt/tests/test_gaussian_pyramid_builder_2d.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vxl_config.h> // for vxl_byte

#include <vil/vil_math.h>
#include <vil/vil_crop.h>
#include <vil/vil_print.h>
#include <vil/vil_decimate.h>
#include <vil/vil_transpose.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_resample_bilin.h>


static void test_resample_bilin()
{
  vcl_cout << "*************************************************\n"
           << " Testing vimt_resample_bilin_smoothed (byte)\n"
           << "*************************************************\n";


  unsigned ni = 20, nj = 20;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<'\n';

  vimt_transform_2d t;
  t.set_origin(vgl_point_2d<double>(ni/2.0, nj/2.0));
  vimt_image_2d_of<float> image0(ni,nj,1,t), image1;

  for (unsigned y=0;y<image0.image().nj();++y)
    for (unsigned x=0;x<image0.image().ni();++x)
      image0.image()(x,y) = x+y*10.0f + 15.0f;

  vcl_cout << "image 0: " << image0 << vcl_setw(3);
  vil_print_all(vcl_cout, image0.image());

  vimt_resample_bilin_smoothing(image0, image1, vgl_point_2d<double>(0,0),
    vgl_vector_2d<double>(1,0), vgl_vector_2d<double>(0,1), ni/2, nj/2);

  vcl_cout << "\n\n\nimage 1: " << image1 << vcl_setw(3);
  vil_print_all(vcl_cout, image1.image());

  
  TEST("similar resolutions sample is ok",
    vil_math_ssd(vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2),
                          image1.image(), double() ), 0);

//  Add pixel-wide checkerboard to test smoothing
  vimt_image_2d_of<float> image2(ni,nj,1,t), image3;

  for (unsigned y=0;y<image2.image().nj();++y)
    for (unsigned x=0;x<image2.image().ni();++x)
      image2.image()(x,y) = x+y*10.0f + 30.0f*((x+y)%2);

  vcl_cout << "image 2: " << image2 << vcl_setw(3) << vcl_right;
  vil_print_all(vcl_cout, image2.image());

  vimt_resample_bilin_smoothing(image2, image3, vgl_point_2d<double>(0,0),
    vgl_vector_2d<double>(0,2), vgl_vector_2d<double>(2,0), ni/4, nj/4);

  vcl_cout << "image 3: " << image3 << vcl_setw(3);
  vil_print_all(vcl_cout, image3.image());

  vcl_cout << "dec(crop (image 0)): " << image3 << vcl_setw(3);
  vil_print_all(vcl_cout, vil_transpose(vil_decimate(vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2), 2)));

  
  TEST_NEAR("different resolutions sample is ok",
    vil_math_ssd(
      vil_transpose(vil_decimate(
        vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2), 2)),
      image3.image(), double() ) / 25.0, 0, 6.0);




}




TESTMAIN(test_resample_bilin);
