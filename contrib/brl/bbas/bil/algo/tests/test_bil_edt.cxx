#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bil/algo/bil_edt.h>

#include <vil/vil_print.h>
#include <vil/vil_copy.h>
#include <vil/vil_image_view.h>


void bil_edt_test(vil_image_view<vxl_uint_32> &im, bool print, bool three_d=false);
void bil_edt_test_3D(vil_image_view<vxl_uint_32> &im, bool print);
void bil_edt_test_specific(const vil_image_view<vxl_uint_32> &im, const vil_image_view<vxl_uint_32>& dt_brute, bool print, const std::string& algo);

#define DATA(I) (I).top_left_ptr()

static void test_bil_edt()
{
  std::cout << "Exact Euclidean Distance Transform Algorithms\n";

  {
  unsigned r=5,c=7;

  vil_image_view <vxl_uint_32> image(r,c,1);

  image.fill(1);

  image(3,2)=0;
  image(0,2)=0;
  image(0,0)=0;
  image(4,4)=0;
  DATA(image)[34]=0;

  std::cout << "ORIGINAL IMAGE:\n" << std::endl;
  vil_print_all(std::cout,image);

  bil_edt_test(image,true);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(0,0) = 0;
  bil_edt_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(99,99) = 0;
  bil_edt_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(0,99) = 0;
  bil_edt_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(99,0) = 0;
  bil_edt_test(image,false);
  }

  { // 8-disconnected Voronoi region that breaks most DT algorithms that claim to be Euclidean
  vil_image_view <vxl_uint_32> image(14,13,1);
  image.fill(1);
  image(0,2)  = 0;
  image(1,6)  = 0;
  image(6,12) = 0;
  bil_edt_test(image,true);
  }

  // ----- 3D -----

  {
  unsigned r=7,c=5,p=3;

  vil_image_view <vxl_uint_32> image(c,r,p);

  image.fill(1);

  image(2,3,1)=0;

  std::cout << "ORIGINAL IMAGE:\n" << std::endl;
  vil_print_all(std::cout,image);

  bil_edt_test(image,true,true);
  }

  { // just runs on big image
  unsigned r=200,c=300,p=400;
  vil_image_view <vxl_uint_32> image(c,r,p);

  image.fill(1);
  image(2,3,1)=0;
  image(200,150,350)=0;
  bil_edt_saito(image);
  }

  {
  unsigned r=7,c=5,p=3;
  vil_image_view <vxl_uint_32> image(c,r,p);
  image.fill(1);

  image(2,3,1)=0;
  image(0,0,0)=0;
  image(4,3,2)=0;
  bil_edt_test(image,false,true);
  }
}

TESTMAIN(test_bil_edt);

// test 2D EDT
void
bil_edt_test(vil_image_view<vxl_uint_32> &im, bool print, bool three_d)
{
  vil_image_view <vxl_uint_32> dt_brute(vil_copy_deep(im));

  if (three_d)
    bil_edt_brute_force(dt_brute);
  else
    bil_edt_brute_force_with_list(dt_brute);

  if (print) {
     std::cout << "BRUTE DT:\n";
     vil_print_all(std::cout,dt_brute);
  }

  if (three_d) {
    bil_edt_test_specific(im, dt_brute, print, "Saito 3D");
  } else {
    bil_edt_test_specific(im, dt_brute, print, "Maurer");
    bil_edt_test_specific(im, dt_brute, print, "Saito");
  }
}

void
bil_edt_test_specific(
    const vil_image_view<vxl_uint_32> &im,
    const vil_image_view<vxl_uint_32>& dt_brute,
    bool print,
    const std::string& algo)
{
  vil_image_view <vxl_uint_32> dt_algo(vil_copy_deep(im));

  if (algo == "Maurer")
    bil_edt_maurer(dt_algo);
  else if (algo == "Saito")
    bil_edt_saito(dt_algo);
  else if (algo == "Saito 3D")
    bil_edt_saito_3D(dt_algo);
  else
    std::abort();

  if (print) {
    std::cout << algo << " DT:\n";
    vil_print_all(std::cout,dt_algo);
  }

  bool algo_error=false;

  for (unsigned i=0; i<im.size(); ++i) {
    unsigned dst;
    dst = DATA(dt_brute)[i];
    if (dst != DATA(dt_algo)[i]) {
      std::cout << "Error! " << algo << ": " << DATA(dt_algo)[i] << " EXACT: " << dst << std::endl;
      algo_error = true;
      break;
    }
  }

  std::string msg = std::string("Is ") + algo + std::string(" exact");
  TEST(msg.c_str() , algo_error, false);
}
