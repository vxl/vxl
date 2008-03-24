#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <bil/bil_bounded_image_view.h>

static void test_bounded_image_view()
{
  unsigned ni = 5, nj = 5;
  unsigned ib0 = 3, jb0=4;
  unsigned nib = 11, njb=11;
  //Create a normal 5 x 5 image view
  vil_image_view<unsigned short> v(ni, nj);
  v.fill(10);
  //Create a bounded image view that is 11 x 11 containing the 5 x 5 data
  bil_bounded_image_view<unsigned short> bv(v, ib0, jb0, nib, njb);
  for (unsigned r = 0; r<njb; ++r)
  {
    for (unsigned c = 0; c<nib; ++c)
      vcl_cout << bv.gpix(c,r) << ' ';
    vcl_cout << '\n';
  }
  bool good = bv.gpix(3,4)==10&&bv.gpix(8,9)==0;
  TEST("bounded image values",good, true);

  //Test that we can successfully cast back the bounded_image_view
  //from the abstract view base pointer.
  vil_image_view_base_sptr nbv =
  new bil_bounded_image_view<unsigned short>(v, ib0, jb0, nib, njb);
  bil_bounded_image_view<unsigned short> rn= nbv;
  TEST("casting test", 11, rn.nib());

  //Test that we can address pixels outside the global image
  good = bv.gpix(4,12)==0&&bv.gpix(12,4)==0;
  TEST("bounded image values outside range",good, true);
}

TESTMAIN(test_bounded_image_view);
