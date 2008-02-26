#include <testlib/testlib_test.h>
//:
// \file
// \author Ricardo Fabbri

#include <vil/algo/vil_exact_distance_transform.h>
#include <vil/vil_print.h>
#include <vil/vil_copy.h>
#include <vcl_cstdlib.h>

void vil_exact_distance_transform_test(vil_image_view<vxl_uint_32> &im, bool print, bool three_d=false);

void
vil_exact_distance_transform_test_3D(vil_image_view<vxl_uint_32> &im, bool print);

void
vil_exact_distance_transform_test_specific(const vil_image_view<vxl_uint_32> &im, const vil_image_view<vxl_uint_32> dt_brute, bool print, vcl_string algo);

#define DATA(I) (I).top_left_ptr()

MAIN( test_algo_exact_distance_transform )
{
  START ("Exact Euclidean Distance Transform Algorithms");

  {
  unsigned r=5,c=7;

  vil_image_view <vxl_uint_32> image(r,c,1);

  image.fill(1);

  image(3,2)=0;
  image(0,2)=0;
  image(0,0)=0;
  image(4,4)=0;
  DATA(image)[34]=0;

  vcl_cout << "ORIGINAL IMAGE:\n" << vcl_endl;
  vil_print_all(vcl_cout,image);

  vil_exact_distance_transform_test(image,true);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(0,0) = 0;
  vil_exact_distance_transform_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(99,99) = 0;
  vil_exact_distance_transform_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(0,99) = 0;
  vil_exact_distance_transform_test(image,false);
  }

  {
  vil_image_view <vxl_uint_32> image(100,100,1);
  image.fill(1);
  image(99,0) = 0;
  vil_exact_distance_transform_test(image,false);
  }

  { // 8-disconnected Voronoi region that breaks most DT algorithms that claim to be Euclidean
  vil_image_view <vxl_uint_32> image(14,13,1);
  image.fill(1);
  image(0,2)  = 0;
  image(1,6)  = 0;
  image(6,12) = 0;
  vil_exact_distance_transform_test(image,true);
  }

  // ----- 3D -----

  {
  unsigned r=7,c=5,p=3;

  vil_image_view <vxl_uint_32> image(c,r,p);

  image.fill(1);

  image(2,3,1)=0;

  vcl_cout << "ORIGINAL IMAGE:\n" << vcl_endl;
  vil_print_all(vcl_cout,image);

  vil_exact_distance_transform_test(image,true,true);
  }


#if 0
  { //: just runs on big image
  unsigned r=200,c=300,p=400;
  vil_image_view <vxl_uint_32> image(c,r,p);

  image.fill(1);
  image(2,3,1)=0;
  image(200,150,350)=0;
  vil_exact_distance_transform_saito(image);
  }
#endif // 0

  {
  unsigned r=7,c=5,p=3;
  vil_image_view <vxl_uint_32> image(c,r,p);
  image.fill(1);

  image(2,3,1)=0;
  image(0,0,0)=0;
  image(4,3,2)=0;
  vil_exact_distance_transform_test(image,false,true);
  }


  SUMMARY();
}

//: test 2D EDT
void
vil_exact_distance_transform_test(vil_image_view<vxl_uint_32> &im, bool print, bool three_d)
{
  vil_image_view <vxl_uint_32> dt_brute(vil_copy_deep(im));

  if (three_d)
    vil_exact_distance_transform_brute_force(dt_brute);
  else
    vil_exact_distance_transform_brute_force_with_list(dt_brute);

  if (print) {
     vcl_cout << "BRUTE DT:\n";
     vil_print_all(vcl_cout,dt_brute);
  }


  if (three_d) {
    vil_exact_distance_transform_test_specific(im, dt_brute, print, "Saito 3D");
  } else {
    vil_exact_distance_transform_test_specific(im, dt_brute, print, "Maurer");
    vil_exact_distance_transform_test_specific(im, dt_brute, print, "Saito");
  }
}

void
vil_exact_distance_transform_test_specific(
    const vil_image_view<vxl_uint_32> &im,
    const vil_image_view<vxl_uint_32> dt_brute,
    bool print,
    vcl_string algo)
{
  vil_image_view <vxl_uint_32> dt_algo(vil_copy_deep(im));

  if (algo == "Maurer")
    vil_exact_distance_transform_maurer(dt_algo);
  else if (algo == "Saito")
    vil_exact_distance_transform_saito(dt_algo);
  else if (algo == "Saito 3D")
    vil_exact_distance_transform_saito_3D(dt_algo);
  else
    vcl_abort();

  if (print) {
    vcl_cout << algo << " DT:\n";
    vil_print_all(vcl_cout,dt_algo);
  }

  bool algo_error=false;

  for (unsigned i=0; i<im.size(); ++i) {
    unsigned dst;
    dst = DATA(dt_brute)[i];
    if (dst != DATA(dt_algo)[i]) {
      vcl_cout << "Error! " << algo << ": " << DATA(dt_algo)[i] << " EXACT: " << dst << vcl_endl;
      algo_error = true;
      break;
    }
  }

  vcl_string msg = vcl_string("Is ") + algo + vcl_string(" exact");
  TEST(msg.c_str() , algo_error, false);
}
