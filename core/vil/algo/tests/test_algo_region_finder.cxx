#include <testlib/testlib_test.h>
#include <vil/algo/vil_region_finder.h>
#include <vil/vil_image_view.h>

void
test_algo_region_finder()
{
  unsigned char const data[] = {
    3, 3, 3, 3, 3, 4, 4, 4,
    3, 3, 3, 3, 3, 4, 4, 5,
    3, 3, 3, 4, 4, 5, 4, 5,
    5, 5, 5, 5, 5, 5, 5, 5 };
  vil_image_view<unsigned char> img( data, 8, 4, 1, 1, 8, 1 );

  vil_region_finder<unsigned char> find( img );

  {
    testlib_test_begin( "Extract region, 4 connected" );
    vcl_vector<unsigned> ri, rj;
    find.same_int_region( 1, 1, ri, rj );
    testlib_test_perform( ri.size() == 13 && rj.size() == 13 );
  }

  {
    testlib_test_begin( "Extract same region again" );
    vcl_vector<unsigned> ri, rj;
    find.same_int_region( 4, 0, ri, rj );
    testlib_test_perform( ri.size() == 0 && rj.size() == 0 );
  }

  {
    testlib_test_begin( "Extract another region, 4 connected" );
    vcl_vector<unsigned> ri, rj;
    find.same_int_region( 3, 2, ri, rj );
    testlib_test_perform( ri.size() == 2 && rj.size() == 2 );
  }

  vil_region_finder<unsigned char> find2( img, vil_region_finder_8_conn );

  {
    testlib_test_begin( "Extract region, 8 connected" );
    vcl_vector<unsigned> ri, rj;
    find2.same_int_region( 3, 2, ri, rj );
    testlib_test_perform( ri.size() == 8 && rj.size() == 8 );
  }
}

TESTMAIN( test_algo_region_finder );

