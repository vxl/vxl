// This is mul/vimt/tests/test_resample_bilin.cxx
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_math.h>
#include <vil/vil_crop.h>
#include <vil/vil_print.h>
#include <vil/vil_decimate.h>
#include <vil/vil_transpose.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_resample_bilin.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

static void test_resample_bilin()
{
  std::cout << "*********************************************\n"
           << " Testing vimt_resample_bilin_smoothed (byte)\n"
           << "*********************************************\n";

  unsigned ni = 20, nj = 20;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<'\n';

  vimt_transform_2d t;
  t.set_origin(vgl_point_2d<double>(ni/2.0, nj/2.0));
  vimt_image_2d_of<float> image0(ni,nj,1,t), image1;

  for (unsigned y=0;y<image0.image().nj();++y)
    for (unsigned x=0;x<image0.image().ni();++x)
      image0.image()(x,y) = x+y*10.0f + 15.0f;

  std::cout << "image 0: " << image0 << std::setw(3);
  vil_print_all(std::cout, image0.image());

  vimt_resample_bilin_smoothing_edge_extend(image0, image1, vgl_point_2d<double>(0,0),
                                            vgl_vector_2d<double>(1,0), vgl_vector_2d<double>(0,1), ni/2, nj/2);

  std::cout << "\n\n\nimage 1: " << image1 << std::setw(3);
  vil_print_all(std::cout, image1.image());

  TEST("similar resolutions sample is ok",
       vil_math_ssd(vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2),
                             image1.image(), double() ), 0);

//  Add pixel-wide checkerboard to test smoothing
  vimt_image_2d_of<float> image2(ni,nj,1,t), image3;

  for (unsigned y=0;y<image2.image().nj();++y)
    for (unsigned x=0;x<image2.image().ni();++x)
      image2.image()(x,y) = x+y*10.0f + (((x+y)%2) ? 30.0f : 0.0f);

  std::cout << "image 2: " << image2 << std::setw(3) << std::right;
  vil_print_all(std::cout, image2.image());

  vimt_resample_bilin_smoothing_edge_extend(image2, image3, vgl_point_2d<double>(0,0),
                                            vgl_vector_2d<double>(0,2), vgl_vector_2d<double>(2,0), ni/4, nj/4);

  std::cout << "image 3: " << image3 << std::setw(3);
  vil_print_all(std::cout, image3.image());

  std::cout << "dec(crop (image 0)): " << image3 << std::setw(3);
  vil_print_all(std::cout, vil_transpose(vil_decimate(vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2), 2)));

  TEST_NEAR("different resolutions sample is ok",
            vil_math_ssd(
              vil_transpose(vil_decimate(
                vil_crop(image0.image(), ni/2, ni/2, nj/2, nj/2), 2)),
              image3.image(), double() ) / 25.0, 0, 6.0);
}

TESTMAIN(test_resample_bilin);
