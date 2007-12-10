// This is core/vil/tests/test_math_median.cxx

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>


static void test_math_median()
{
  vcl_cout << "******************************\n"
           << " Testing vil_math_median\n"
           << "******************************\n";

  vxl_byte median;
  {
    vil_image_view<vxl_byte> img(0,0);
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 0x0 image", median, 0 );
  }

  {
    vil_image_view<vxl_byte> img(1,1);
    img.fill( 5 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 1x1 image, value 5", median, 5 );
    img.fill( 0 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 1x1 image, value 0", median, 0 );
    img.fill( 255 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 1x1 image, value 255", median, 255 );
  }

  {
    vil_image_view<vxl_byte> img(2,1);
    img.fill( 5 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 5", median, 5 );

    img(0,0) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 5,16", median, 10 );

    img(0,0) = 21;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 5,21", median, 13 );

    img(0,0) = 6;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 5,6", median, 5 );

    img(0,0) = 0;
    img(1,0) = 1;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 0,1", median, 0 );

    img(0,0) = 0;
    img(1,0) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 0,16", median, 8 );

    img(0,0) = 254;
    img(1,0) = 255;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 254,255", median, 254 );

    img(0,0) = 200;
    img(1,0) = 255;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 200,255", median, 227 );

    img(0,0) = 0;
    img(1,0) = 255;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 2x1 image, value 0,255", median, 127 );
  }

  {
    vil_image_view<vxl_byte> img(3,3);
    img.fill( 5 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x3 image, value 9x5", median, 5 );

    img(0,1) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x3 image, value 8x5,1x16", median, 5 );

    for( unsigned i = 0; i < 3; ++i ) {
      img(i,0) = 16;
    }
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x3 image, value 5x5,4x16", median, 5 );

    img(1,1) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x3 image, value 4x5,5x16", median, 16 );

    img(2,0) = 12;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x3 image, value 4x5,1x12,4x16", median, 12 );
  }

  {
    vil_image_view<vxl_byte> img(12,1);
    img.fill( 5 );
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 12x5", median, 5 );

    img(0,0) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 11x5,1x16", median, 5 );

    for( unsigned i = 0; i < 5; ++i ) {
      img(i,0) = 16;
    }
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 7x5,5x16", median, 5 );

    img(5,0) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 6x5,6x16", median, 10 );

    img(6,0) = 16;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 5x5,7x16", median, 16 );

    img(2,0) = 12;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 5x5,1x12,6x16", median, 14 );

    img(3,0) = 12;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 5x5,2x12,5x16", median, 12 );

    img(2,0) = 1;
    img(3,0) = 20;
    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 12x1 image, value 1x1,5x5,5x16,1x20", median, 10 );
  }


  {
    vil_image_view<vxl_byte> img(3,1,3);

    img(0,0,0) = 5;   img(1,0,0) = 8;   img(2,0,0) = 8;
    img(0,0,1) = 6;   img(1,0,1) = 5;   img(2,0,1) = 6;
    img(0,0,2) = 3;   img(1,0,2) = 3;   img(2,0,2) = 5;

    vil_math_median( median, img, 0 );
    TEST_EQUAL( "Median of 3x1 image, plane 0", median, 8 );
    vil_math_median( median, img, 1 );
    TEST_EQUAL( "Median of 3x1 image, plane 1", median, 6 );
    vil_math_median( median, img, 2 );
    TEST_EQUAL( "Median of 3x1 image, plane 2", median, 3 );
  }
}

TESTMAIN(test_math_median);
