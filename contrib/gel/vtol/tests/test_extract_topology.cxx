#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil/vil_image_view.h>
#include <testlib/testlib_test.h>
#include <vxl_config.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_intensity_face.h>

#include <vtol/algo/vtol_extract_topology.h>

typedef vxl_uint_16 TEST_LABEL_TYPE;


typedef vil_image_view<vxl_byte> image_type;
typedef vtol_extract_topology< TEST_LABEL_TYPE >::label_image_type label_image_type;

// This class has access to the private member is_edge() of vtol_extract_topology
//
class test_vtol_extract_topology
{
 public:
  static void test_image_1a();
  static void test_image_1b();
};

// since the pixel type and the label type are now
// separated, this converts the former to the latter
// for testing purposes
//
static label_image_type
image_to_label(const image_type& img)
{
  const int nplanes = 1;
  label_image_type img2(img.ni(), img.nj(), nplanes);
  for (unsigned int i=0; i<img.ni(); i++) {
    for (unsigned int j=0; j<img.nj(); j++) {
      img2(i, j) = img(i, j);
    }
  }
  return img2;
}

static image_type
image_1a()
{
  static vxl_byte image_data[] =
    { 1, 1, 1, 2, 1, 1,
      1, 0, 0, 0, 0, 1,
      1, 2, 2, 0, 0, 1,
      1, 1, 0, 2, 2, 1,
      1, 0, 3, 2, 1, 1,
      3, 2, 1, 3, 3, 3 };

  return image_type( image_data, 6, 6, 1, 1, 6, 0 );
}

static image_type
image_1b()
{
  static vxl_byte image_data[] =
    { 0, 0, 0, 0, 0, 0,
      0, 0, 0, 2, 2, 0,
      0, 0, 0, 2, 2, 0,
      0, 1, 1, 0, 0, 0,
      0, 1, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 0 };

  return image_type( image_data, 6, 6, 1, 1, 6, 0 );
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

  return image_type( image_data, 6, 6, 1, 1, 6, 0 );
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

  return image_type( image_data, 8, 6, 1, 1, 8, 0 );
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

  return image_type( image_data, 8, 6, 1, 1, 8, 0 );
}

// min label = 3, non-contiguous labels
static image_type
image_5()
{
  static vxl_byte image_data[] =
    { 3, 5, 5, 5, 5, 5,
      3, 3, 5, 5, 5, 5,
      3, 3, 3, 5, 5, 5,
      3, 3, 3, 3, 5, 5,
      3, 3, 3, 3, 3, 5,
      3, 3, 3, 3, 3, 3 };

  return image_type( image_data, 6, 6, 1, 1, 6, 0);
}


static image_type
image_6()
{
  static vxl_byte image_data[] =
    { 0, 0, 0, 0, 0, 0,
      0, 0, 0, 2, 2, 0,
      0, 0, 1, 0, 0, 0,
      0, 1, 1, 0, 0, 0,
      0, 1, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 0 };

  return image_type( image_data, 6, 6, 1, 1, 6, 0 );
}


void
test_vtol_extract_topology::
test_image_1a()
{
  vcl_cout << "\n\nTesting image 1a\n";

  label_image_type img( image_to_label( image_1a() ) );

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
  vtol_extract_topology<TEST_LABEL_TYPE> te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Testing (internal) is_edge()" );
  bool good = true;
  for ( unsigned i = 0; i < 7; ++i ) {
    for ( unsigned j = 0; j < 7; ++j ) {
      for ( unsigned dir = 0; dir < 4; ++dir ) {
        unsigned d = 1 << dir;
        if ( (edges[j][i] & d) != 0 ) {
          if ( ! te.is_edge( i, j, dir ) ) {
            good = false;
            vcl_cout << "["<<edges[j][i]<<"] ("<<i<<","<<j<<", d="<<dir<<")  should be a boundary edge\n";
          }
        } else {
          if ( te.is_edge( i, j, dir ) ) {
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


void
test_vtol_extract_topology::
test_image_1b()
{
  vcl_cout << "\n\nTesting image 1b\n";

  label_image_type img = image_to_label( image_1b() );

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
  const unsigned LRUD = L | R | U | D;

  static unsigned edges[7][7] =
    { { RD, LR, LR, LR,  LR, LR, LD },
      { UD, 0,  0,  RD,  LR, LD, UD },
      { UD, 0,  0,  UD,  0,  UD, UD },
      { UD, RD, LR, LRUD,LR, LU, UD },
      { UD, UD, 0,  UD,  0,  0, UD },
      { UD, RU, LR, LU,  0,  0, UD },
      { RU, LR, LR, LR, LR, LR, LU }
    };
  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Testing (internal) is_edge()" );
  bool good = true;
  for ( unsigned i = 0; i < 7; ++i )
    for ( unsigned j = 0; j < 7; ++j )
      for ( unsigned dir = 0; dir < 4; ++dir )
      {
        unsigned d = 1 << dir;
        if ( (edges[j][i] & d) != 0 ) {
          if ( ! te.is_edge( i, j, dir ) ) {
            good = false;
            vcl_cout << '['<<edges[j][i]<<"] ("<<i<<','<<j<<", d="<<dir<<")  should be a boundary edge\n";
          }
        } else {
          if ( te.is_edge( i, j, dir ) ) {
            good = false;
            vcl_cout << '['<<edges[j][i]<<"] ("<<i<<','<<j<<", d="<<dir<<")  should not be a boundary edge\n";
          }
        }
      }
  testlib_test_perform( good );

  TEST_EQUAL( "Vertex count", te.vertices().size(), 5 );
  TEST_EQUAL( "Face count", te.faces().size(), 3 );
}


static void
test_image_2()
{
  vcl_cout << "\n\nTesting image 2\n";

  label_image_type img( image_to_label(image_2()) );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Vertex count" );
  testlib_test_perform( te.vertices().size() >= 2 );
  TEST_EQUAL( "Face count", te.faces().size(), 2 );
}


static void
test_image_3()
{
  vcl_cout << "\n\nTesting image 3\n";

  label_image_type img( image_to_label(image_3()) );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( img );
  testlib_test_perform( true );

  testlib_test_begin( "Vertex count" );
  testlib_test_perform( te.vertices().size() >= 3 );
  TEST_EQUAL( "Face count", te.faces().size(), 4 );
}


static void
test_image_4()
{
  vcl_cout << "\n\nTesting image 4\n";

  label_image_type img( image_to_label(image_4()) );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( img );
  testlib_test_perform( true );

  TEST_EQUAL( "Face count", te.faces().size(), 4 );
}


static void
test_image_6()
{
  vcl_cout << "\n\nTesting image 6\n";

  label_image_type img( image_to_label(image_6()) );

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( img );
  testlib_test_perform( true );

  TEST_EQUAL( "Face count", te.faces().size(), 3 );
}


static
bool
has_vertex( vtol_intensity_face_sptr face, double x, double y )
{
  static const double tol = 1e-5;

  vertex_list* verts = face->outside_boundary_vertices();
  for (vertex_list::iterator itr = verts->begin();
       itr != verts->end(); ++itr )
  {
    vtol_vertex_2d* v2d = (*itr)->cast_to_vertex_2d();
    if ( !v2d ) {
      vcl_cerr << "Error: Non-2D vertex in face\n";
      delete verts;
      return false;
    }
    if ( vcl_abs( v2d->x() - x ) < tol &&
         vcl_abs( v2d->y() - y ) < tol ) {
      delete verts;
      return true;
    }
  }
  delete verts;
  return false;
}


static void
test_smoothing()
{
  // Make sure that smoothing doesn't move the vertices around

  vcl_cout << "\n\nTesting edgel chain smoothing\n";

  {
    vcl_cout << "Without smoothing\n";
    label_image_type img( image_to_label(image_5()) );

    testlib_test_begin( "Constructing extract object" );
    vtol_extract_topology< TEST_LABEL_TYPE >te( img );
    testlib_test_perform( true );

    vcl_vector< vtol_intensity_face_sptr > const& faces = te.faces();

    TEST_EQUAL( "Face count", faces.size(), 2 );

    testlib_test_begin( "Face 0, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[0], 0.5, -0.5 ) );
    testlib_test_begin( "Face 0, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[0], 5.5,  4.5 ) );

    testlib_test_begin( "Face 1, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[1], 0.5, -0.5 ) );
    testlib_test_begin( "Face 1, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[1], 5.5,  4.5 ) );
  }

  {
    vcl_cout << "Small smoothing (3)\n";
    label_image_type img( image_to_label(image_5()) );

    testlib_test_begin( "Constructing extract object" );
    vtol_extract_topology< TEST_LABEL_TYPE > te( img,
                              vtol_extract_topology_params().set_smooth( 3 ) );
    testlib_test_perform( true );

    vcl_vector< vtol_intensity_face_sptr > const& faces = te.faces();

    TEST_EQUAL( "Face count", faces.size(), 2 );

    testlib_test_begin( "Face 0, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[0], 0.5, -0.5 ) );
    testlib_test_begin( "Face 0, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[0], 5.5,  4.5 ) );

    testlib_test_begin( "Face 1, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[1], 0.5, -0.5 ) );
    testlib_test_begin( "Face 1, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[1], 5.5,  4.5 ) );
  }

  {
    vcl_cout << "Excessive smoothing (8)\n";
    label_image_type img( image_to_label(image_5()) );

    testlib_test_begin( "Constructing extract object" );
    vtol_extract_topology <TEST_LABEL_TYPE > 
      te( img,vtol_extract_topology_params().set_smooth( 8 ) );
    testlib_test_perform( true );

    vcl_vector< vtol_intensity_face_sptr > const& faces = te.faces();

    TEST_EQUAL( "Face count", faces.size(), 2 );

    testlib_test_begin( "Face 0, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[0], 0.5, -0.5 ) );
    testlib_test_begin( "Face 0, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[0], 5.5,  4.5 ) );

    testlib_test_begin( "Face 1, Vertex (0.5,-0.5)" );
    testlib_test_perform( has_vertex( faces[1], 0.5, -0.5 ) );
    testlib_test_begin( "Face 1, Vertex (5.5, 4.5)" );
    testlib_test_perform( has_vertex( faces[1], 5.5,  4.5 ) );
  }
}


static void
test_digital_region()
{
  // Make sure the digital regions match the segmented regions.

  vcl_cout << "\n\nTesting digital region\n";

  image_type img( image_2() );
  label_image_type label_img = image_to_label(img);

  testlib_test_begin( "Constructing extract object" );
  vtol_extract_topology< TEST_LABEL_TYPE > te( label_img );
  testlib_test_perform( true );

  vcl_vector< vtol_intensity_face_sptr > const& faces = te.faces( img );

  testlib_test_begin( "Vertex count" );
  testlib_test_perform( te.vertices().size() >= 2 );
  TEST_EQUAL( "Face count", te.faces().size(), 2 );

  vdgl_digital_region* reg1 = faces[0]->cast_to_digital_region();
  vdgl_digital_region* reg2 = faces[1]->cast_to_digital_region();

  testlib_test_begin( "Faces have digital regions" );
  testlib_test_perform( reg1 && reg2 );

  int num1 = reg1->Npix();
  int num2 = reg2->Npix();

  testlib_test_begin( "Pixel counts" );
  testlib_test_perform( ( num1 == 10 && num2 == 26 ) ||
                        ( num2 == 10 && num1 == 26 ) );
}


static void
test_extract_topology()
{
  test_vtol_extract_topology::test_image_1a();
  test_vtol_extract_topology::test_image_1b();
  test_image_2();
  test_image_3();
  test_image_4();
  test_smoothing();
  test_digital_region();
  test_image_6();
}


TESTMAIN(test_extract_topology);
