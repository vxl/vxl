// This is core/vil/algo/tests/test_algo_threshold.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_checker_board.h>
#include <vil/vil_image_view.h>

static void test_algo_checker_board()
{
  // create two image views 
  // One has rectangle  (40, 40) - (140, 80)
  // One has rectangle  (40, 40) - (80, 140)
  
  vil_image_view<vxl_byte> a(200, 200), b(200,200);
  a.fill( vxl_byte(0) );
  b.fill( vxl_byte(0) );
  
  for( unsigned i=40; i<140; ++i )
    for( unsigned j=40; j<80; ++j ) {
      a(i, j) = 100;
      b(j, i) = 200;
    }
    
  // setup
  vil_image_view<vxl_byte> des;
  vil_checker_board_params params( 10, 10 );
  params.not_use_color( 0 );
  
  vil_checker_board( des, a, b, params );
  
  TEST("Black area", des(35,35), vxl_byte(0) );
  TEST("Black area", des(105,105), vxl_byte(0) );
  TEST("First Block", des(45,45), vxl_byte(100) );
  TEST("Second Block", des(55,45), vxl_byte(200) );
  TEST("Second Block", des(45,55), vxl_byte(200) );
  TEST("Diagonal Block", des(55,55), vxl_byte(100) );

  TEST("Only 1st image", des(115,45), vxl_byte(100) );
  TEST("Only 2nd image", des(45,115), vxl_byte(200) );
}

TESTMAIN(test_algo_checker_board);
