#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vil/vil_image_view.h>

#include <testlib/testlib_test.h>

#include <vxl_config.h>

#include <vtol/algo/vtol_extract_topology.h>

typedef vil_image_view<vxl_byte> image_type;


// This class has access to the private members of vtol_extract_topology
//
class test_vtol_extract_topology
{
public:
  static void test_image_1();
};


static image_type
image_1()
{
  static vxl_byte image_data[] =
    { 1, 1, 1, 2, 1, 1,
      1, 0, 0, 0, 0, 1,
      1, 2, 2, 0, 0, 1,
      1, 1, 0, 2, 2, 1,
      1, 0, 3, 2, 1, 1,
      3, 2, 1, 3, 3, 3 };

  return image_type( image_data, 6, 6, 1, 1, 6, 6*6 );
}


static image_type
image_2()
{
  static vxl_byte image_data[] =
    { 1, 1, 1, 1, 1, 1,
      1, 1, 0, 0, 0, 1,
      1, 1, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 1,
      1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1 };

  return image_type( image_data, 6, 6, 1, 1, 6, 6*6 );
}


static image_type
image_3()
{
  static vxl_byte image_data[] =
    { 1, 1, 1, 1, 1, 1, 1, 1,
      1, 0, 0, 0, 0, 1, 1, 1,
      1, 0, 1, 1, 0, 1, 0, 1,
      1, 0, 1, 1, 0, 1, 0, 1,
      1, 0, 0, 0, 0, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1 };

  return image_type( image_data, 8, 6, 1, 1, 8, 8*6 );
}

// min label = 3, non-contiguous labels
static image_type
image_4()
{
  static vxl_byte image_data[] =
    { 3, 5, 5, 5, 3, 3, 3, 3,
      3, 5, 5, 5, 3, 3, 3, 3,
      3, 5, 5, 5, 3, 3, 3, 3,
      3, 3, 3, 3, 5, 5, 5, 5,
      3, 3, 3, 3, 5, 5, 5, 5,
      3, 3, 3, 3, 3, 3, 3, 3 };

  return image_type( image_data, 8, 6, 1, 1, 8, 8*6 );
}


void
test_vtol_extract_topology::
test_image_1( )
{
  vcl_cout << "\n\nTesting image 1" << vcl_endl;

  image_type img( image_1() );

  // edge directions
  const unsigned R = 1;
  const unsigned D = 2;
  const unsigned L = 4;
  const unsigned U = 8;
  const unsigned LD = L | D; 
  const unsigned LR = L | R;
  const unsigned LU = L | U;
  const unsigned RD = R | D;
  const unsigned RU = R | U;
  const unsigned UD = U | D;
  const unsigned LUD = L | U | D;
  const unsigned RUD = R | U | D;
  const unsigned LRD = L | R | D;
  const unsigned LRU = L | R | U;
  const unsigned LRUD = L | R | U | D;
  static unsigned edges[7][7] =
    { { RD, LR, LR, LRD, LRD, LR, LD },
      { UD, RD, LR, LRU, LRU, LD, UD },
      { UD, RUD, LR, LD, 0, UD, UD },
      { UD, RU, LRD, LRUD, LR, LUD, UD },
      { UD, RD, LRUD, LUD, RD, LU, UD },
      { RUD, LRUD, LRUD, LRUD, LRU, LR, LUD },

      { RU, LRU, LRU, LRU, LR, LR, LU } };

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Testing (internal) is_edge()" );
  bool good = true;
  for( unsigned i = 0; i < 7; ++i ) {
    for( unsigned j = 0; j < 7; ++j ) {
      for( unsigned dir = 0; dir < 4; ++dir ) {
        unsigned d = 1 << dir;
        if( (edges[j][i] & d) != 0 ) {
          if( ! te.is_edge( i, j, dir ) ) {
            good = false;
            vcl_cout << "["<<edges[j][i]<<"] ("<<i<<","<<j<<", d="<<dir<<")  should be a boundary edge\n";
          }
        } else {
          if( te.is_edge( i, j, dir ) ) {
            good = false;
            vcl_cout << "["<<edges[j][i]<<"] ("<<i<<","<<j<<", d="<<dir<<")  should not be a boundary edge\n";
          }
        }
      }
    }
  }
  testlib_test_perform( good );

  TEST_EQUAL( "Vertex count", te.vertices().size(), 19 );
  TEST_EQUAL( "Face count", te.faces().size(), 13 );
}


static void
test_image_2( )
{
  vcl_cout << "\n\nTesting image 2" << vcl_endl;

  image_type img( image_2() );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Vertex count" );
  testlib_test_perform( te.vertices().size() >= 2 );
  TEST_EQUAL( "Face count", te.faces().size(), 2 );
}


static void
test_image_3( )
{
  vcl_cout << "\n\nTesting image 3" << vcl_endl;

  image_type img( image_3() );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Vertex count" );
  testlib_test_perform( te.vertices().size() >= 3 );
  TEST_EQUAL( "Face count", te.faces().size(), 4 );
}

static void
test_image_4( )
{
  vcl_cout << "\n\nTesting image 4" << vcl_endl;

  image_type img( image_4() );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology te( img );
  testlib_test_perform( true );

  TEST_EQUAL( "Face count", te.faces().size(), 4 );
}


static void
test_extract_topology()
{
  test_vtol_extract_topology::test_image_1();
  test_image_2();
  test_image_3();
  test_image_4();
}


TESTMAIN(test_extract_topology);
