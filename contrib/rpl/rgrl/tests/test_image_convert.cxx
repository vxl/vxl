#include <testlib/testlib_test.h>

#include <rgrl/rgrl_image_convert.h>

namespace {

  void test_3d() 
  {
    unsigned ni = 20;
    unsigned nj = 20;
    unsigned nk = 20;
    vil3d_image_view<vxl_byte> image0;
    image0.set_size(ni,nj,nk);
    vil3d_image_view<vxl_byte> image2;

    for (unsigned y=0;y<image0.nj();++y)
      for (unsigned x=0;x<image0.ni();++x)
        for (unsigned z=0;z<image0.nk();++z)
          image0(x,y,z) = z ;
    
    rgrl_image_convert_3d( image0, 1,1, 1.5, image2 ); 
    testlib_test_begin( "Test sizes" );
    testlib_test_perform( image2.ni() == 20 );
    testlib_test_perform( image2.nj() == 20 );
    testlib_test_perform( image2.nk() == 30 );

    testlib_test_begin( "Test boundary pixels" );
    testlib_test_perform( image2(0, 0, 29 ) == 0 );

    testlib_test_begin( "Test interpolated pixels" );
    testlib_test_perform( image2(10, 10, 15 ) == 10 );
  }
}

MAIN( test_image_convert )
{
  START( "test_image_convert" );
  test_3d();

  SUMMARY();
}
