// This is mul/vimt3d/tests/test_reflect.cxx

#include <iostream>
#include <ctime>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>
#include <vimt3d/vimt3d_reflect.h>
#include <vul/vul_ios_state.h>


//========================================================================
// Test vimt3d_reflect() in i, j and k directions
//========================================================================
static void test_reflect_int()
{
  vul_ios_state_saver ios_ss(std::cout);
  std::cout.precision(9);

  std::cout << "***********************************\n"
           << " Testing vimt3d_reflect<vxl_int_32>\n"
           << "***********************************\n";

  constexpr unsigned ni = 5;
  constexpr unsigned nj = 5;
  constexpr unsigned nk = 5;

  vimt3d_image_3d_of<vxl_int_32> image(ni, nj, nk);
  for (unsigned k=0; k<nk; ++k)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned i=0; i<ni; ++i)
      {
        image.image()(i,j,k) = 100*k + 10*j + i;
      }
    }
  }
  double sx=1.0, sy=1.1, sz=1.2;
  double tx=0.0, ty=-100.0, tz=200.0;
  image.world2im().set_zoom_only(sx, sy, sz, tx, ty, tz);

#ifndef NDEBUG
  {
    std::cout << "\nimage:\n";
    image.print_summary(std::cout);
    std::cout << "vimt3d_voxel_size_from_transform: " << vimt3d_voxel_size_from_transform(image) << "\n";
    vgl_box_3d<double> bbox = world_bounding_box(image);
    std::cout << "world_bounding_box: " << bbox << "\n";
    std::cout << "centre: " << bbox.centroid() << "\n";
    std::cout << "world2im.origin: " << image.world2im().origin() << "\n";
    std::cout << "im2world.origin: " << image.world2im().inverse().origin() << "\n";
    std::cout << std::endl;
  }
#endif //NDEBUG

  vimt3d_image_3d_of<vxl_int_32> tempx, tempy, tempz;
  tempx.deep_copy(image);
  tempy.deep_copy(image);
  tempz.deep_copy(image);
  vimt3d_reflect_x(tempx);
  vimt3d_reflect_y(tempy);
  vimt3d_reflect_z(tempz);

#ifndef NDEBUG
  {
    std::cout << "\ntempx:\n";
    tempx.print_summary(std::cout);
    std::cout << "vimt3d_voxel_size_from_transform: " << vimt3d_voxel_size_from_transform(tempx) << "\n";
    vgl_box_3d<double> bbox = world_bounding_box(tempx);
    std::cout << "world_bounding_box: " << bbox << "\n";
    std::cout << "centre: " << bbox.centroid() << "\n";
    std::cout << "world2im.origin: " << tempx.world2im().origin() << "\n";
    std::cout << "im2world.origin: " << tempx.world2im().inverse().origin() << "\n";
    std::cout << std::endl;
  }
  {
    std::cout << "\ntempy:\n";
    tempy.print_summary(std::cout);
    std::cout << "vimt3d_voxel_size_from_transform: " << vimt3d_voxel_size_from_transform(tempy) << "\n";
    vgl_box_3d<double> bbox = world_bounding_box(tempy);
    std::cout << "world_bounding_box: " << bbox << "\n";
    std::cout << "centre: " << bbox.centroid() << "\n";
    std::cout << "world2im.origin: " << tempy.world2im().origin() << "\n";
    std::cout << "im2world.origin: " << tempy.world2im().inverse().origin() << "\n";
    std::cout << std::endl;
  }
  {
    std::cout << "\ntempz:\n";
    tempz.print_summary(std::cout);
    std::cout << "vimt3d_voxel_size_from_transform: " << vimt3d_voxel_size_from_transform(tempz) << "\n";
    vgl_box_3d<double> bbox = world_bounding_box(tempz);
    std::cout << "world_bounding_box: " << bbox << "\n";
    std::cout << "centre: " << bbox.centroid() << "\n";
    std::cout << "world2im.origin: " << tempz.world2im().origin() << "\n";
    std::cout << "im2world.origin: " << tempz.world2im().inverse().origin() << "\n";
    std::cout << std::endl;
  }
#endif //NDEBUG

  bool refl_i_ok = true;
  bool refl_j_ok = true;
  bool refl_k_ok = true;
  vgl_point_3d<double> origin(0,0,0);
  for (unsigned k=0; k<nk; ++k)
  {
    unsigned rk = nk-1-k;
    for (unsigned j=0; j<nj; ++j)
    {
      unsigned rj = nj-1-j;
      for (unsigned i=0; i<ni; ++i)
      {
        unsigned ri = ni-1-i;

        // Check voxel data
        refl_i_ok = refl_i_ok && (tempx.image()(i, j, k) == image.image()(ri, j, k));
        refl_j_ok = refl_j_ok && (tempy.image()(i, j, k) == image.image()(i, rj, k));
        refl_k_ok = refl_k_ok && (tempz.image()(i, j, k) == image.image()(i, j, rk));

        // Check transform
        // For the x-reflected image, the x-coord of each voxel's world position should be the exact
        // negative of the x-coord of the world position of the opposite voxel in the original image.
        // Similarly for the y- and z- reflected images.
        refl_i_ok = refl_i_ok && std::fabs((tempx.world2im().inverse()(vgl_point_3d<double>(i, j, k))).x() -
          -(image.world2im().inverse()(vgl_point_3d<double>(ri, j, k))).x())<1e-9;
        refl_j_ok = refl_j_ok && std::fabs((tempy.world2im().inverse()(vgl_point_3d<double>(i, j, k))).y() -
          -(image.world2im().inverse()(vgl_point_3d<double>(i, rj, k))).y())<1e-9;
        refl_k_ok = refl_k_ok && std::fabs((tempz.world2im().inverse()(vgl_point_3d<double>(i, j, k))).z() -
          -(image.world2im().inverse()(vgl_point_3d<double>(i, j, rk))).z())<1e-9;
      }
    }
  }

  TEST("vimt3d_reflect_x", refl_i_ok, true);
  TEST("vimt3d_reflect_y", refl_j_ok, true);
  TEST("vimt3d_reflect_z", refl_k_ok, true);
}


//========================================================================
// Main testing function
//========================================================================
static void test_reflect()
{
  test_reflect_int();
}

TESTMAIN(test_reflect);
