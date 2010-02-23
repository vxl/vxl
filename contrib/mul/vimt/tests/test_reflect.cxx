// This is mul/vimt/tests/test_reflect.cxx

#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_ctime.h>
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vimt/vimt_reflect.h>
#include <vul/vul_ios_state.h>


//========================================================================
// Test vimt_reflect() in i, j and k directions
//========================================================================
static void test_reflect_int()
{
  vul_ios_state_saver ios_ss(vcl_cout);
  vcl_cout.precision(9);

  vcl_cout << "***********************************\n"
           << " Testing vimt_reflect<vxl_int_32>\n"
           << "***********************************\n";

  const unsigned ni = 5;
  const unsigned nj = 5;

  vimt_image_2d_of<vxl_int_32> image(ni, nj);
  for (unsigned j=0; j<nj; ++j)
  {
    for (unsigned i=0; i<ni; ++i)
    {
      image.image()(i,j) = 10*j + i;
    }
  }
  double sx=1.0, sy=1.1;
  double tx=0.0, ty=-100.0;
  image.world2im().set_zoom_only(sx, sy, tx, ty);

#ifndef NDEBUG
  {
    vcl_cout << "\nimage:\n";
    image.print_summary(vcl_cout);
    vcl_cout << "vimt_pixel_size_from_transform: " << vimt_pixel_size_from_transform(image) << "\n";
    vgl_box_2d<double> bbox = world_bounding_box(image);
    vcl_cout << "world_bounding_box: " << bbox << "\n";
    vcl_cout << "centre: " << bbox.centroid() << "\n";
    vcl_cout << "world2im.origin: " << image.world2im().origin() << "\n";
    vcl_cout << "im2world.origin: " << image.world2im().inverse().origin() << "\n";
    vcl_cout << vcl_endl;
  }
#endif //NDEBUG

  vimt_image_2d_of<vxl_int_32> tempx, tempy;
  tempx.deep_copy(image);
  tempy.deep_copy(image);
  vimt_reflect_x(tempx);
  vimt_reflect_y(tempy);

#ifndef NDEBUG
  {
    vcl_cout << "\ntempx:\n";
    tempx.print_summary(vcl_cout);
    vcl_cout << "vimt_pixel_size_from_transform: " << vimt_pixel_size_from_transform(tempx) << "\n";
    vgl_box_2d<double> bbox = world_bounding_box(tempx);
    vcl_cout << "world_bounding_box: " << bbox << "\n";
    vcl_cout << "centre: " << bbox.centroid() << "\n";
    vcl_cout << "world2im.origin: " << tempx.world2im().origin() << "\n";
    vcl_cout << "im2world.origin: " << tempx.world2im().inverse().origin() << "\n";
    vcl_cout << vcl_endl;
  }
  {
    vcl_cout << "\ntempy:\n";
    tempy.print_summary(vcl_cout);
    vcl_cout << "vimt_pixel_size_from_transform: " << vimt_pixel_size_from_transform(tempy) << "\n";
    vgl_box_2d<double> bbox = world_bounding_box(tempy);
    vcl_cout << "world_bounding_box: " << bbox << "\n";
    vcl_cout << "centre: " << bbox.centroid() << "\n";
    vcl_cout << "world2im.origin: " << tempy.world2im().origin() << "\n";
    vcl_cout << "im2world.origin: " << tempy.world2im().inverse().origin() << "\n";
    vcl_cout << vcl_endl;
  }
#endif //NDEBUG

  bool refl_i_ok = true;
  bool refl_j_ok = true;
  vgl_point_2d<double> origin(0,0);
  for (unsigned j=0; j<nj; ++j)
  {
    unsigned rj = nj-1-j;
    for (unsigned i=0; i<ni; ++i)
    {
      unsigned ri = ni-1-i;
      
      // Check pixel data
      refl_i_ok = refl_i_ok && (tempx.image()(i, j) == image.image()(ri, j));
      refl_j_ok = refl_j_ok && (tempy.image()(i, j) == image.image()(i, rj));
      
      // Check transform
      // For the x-reflected image, the x-coord of each pixel's world position should be the exact
      // negative of the x-coord of the world position of the opposite pixel in the original image.
      // Similarly for the y-reflected image.
      refl_i_ok = refl_i_ok && vcl_fabs((tempx.world2im().inverse()(vgl_point_2d<double>(i, j))).x() -
        -(image.world2im().inverse()(vgl_point_2d<double>(ri, j))).x())<1e-9;
      refl_j_ok = refl_j_ok && vcl_fabs((tempy.world2im().inverse()(vgl_point_2d<double>(i, j))).y() -
        -(image.world2im().inverse()(vgl_point_2d<double>(i, rj))).y())<1e-9;
    }
  }

  TEST("vimt_reflect_x", refl_i_ok, true);
  TEST("vimt_reflect_y", refl_j_ok, true);
}


//========================================================================
// Main testing function
//========================================================================
static void test_reflect()
{
  test_reflect_int();
}

TESTMAIN(test_reflect);
