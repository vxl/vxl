#include <vgui/internals/vgui_generic_vil_image_view.h>

#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>

#include <vxl_config.h>

#define TestSingleWith( T , w , h , p , c )                             \
{                                                                       \
  {                                                                     \
    vil_image_view< T > img( w, h, p , c );                             \
    vgui_generic_vil_image_view generic( img );                         \
    TEST( "top left", img.top_left_ptr(), (T const*)generic.top_left_ptr() ); \
    TEST( "istep", img.istep(), generic.istep() );                      \
    TEST( "jstep", img.jstep(), generic.jstep() );                      \
    TEST( "planestep", img.planestep(), generic.planestep() );          \
    TEST( "memory ptr", img.memory_chunk(), generic.memory_chunk() );   \
    TEST( "image view", img, *generic.make_view() );                    \
  }                                                                     \
  {                                                                     \
    vcl_cout << "  from operator== " << #T << '\n';                     \
    vil_image_view< T > img( w, h, p , c );                             \
    vgui_generic_vil_image_view generic;                                \
    generic = img;                                                      \
    TEST( "top left", img.top_left_ptr(), (T const*)generic.top_left_ptr() ); \
    TEST( "istep", img.istep(), generic.istep() );                      \
    TEST( "jstep", img.jstep(), generic.jstep() );                      \
    TEST( "planestep", img.planestep(), generic.planestep() );          \
    TEST( "memory ptr", img.memory_chunk(), generic.memory_chunk() );   \
    TEST( "image view", img, *generic.make_view() );                    \
  }                                                                     \
}

#define TestWith( T )                                                   \
{                                                                       \
  vcl_cout << "Testing pixel type " << #T << '\n';                      \
  vcl_cout << "  from \"grey\" constructor " << #T << '\n';             \
  TestSingleWith( T , 5 , 6 , 7 , 1 );                                  \
  vcl_cout << "  from \"RGB\" constructor " << #T << '\n';              \
  TestSingleWith( T , 5 , 6 , 1 , 3 );                                  \
  vcl_cout << "  from constructor vil_rgb<" << #T << ">\n";             \
  TestSingleWith( vil_rgb<T > , 5 , 6 , 1 , 1 );                        \
  vcl_cout << "  from constructor vil_rgba<" << #T << ">\n";            \
  TestSingleWith( vil_rgba<T > , 5 , 6 , 1 , 1 );                       \
}

void
test_generic_image_view()
{
  // These tests make sure that the data stored in the generic image
  // are the same as that of the image used to create it.

#if VXL_HAS_INT_64
  TestWith( vxl_uint_64 );
  TestWith( vxl_int_64 );
#endif
  TestWith( vxl_uint_32 );
  TestWith( vxl_int_32 );
  TestWith( vxl_uint_16 );
  TestWith( vxl_int_16 );
  TestWith( vxl_byte );
  TestWith( vxl_sbyte );
  TestWith( double );
  TestWith( float );
  vcl_cout << "Testing pixel type bool\n  from \"grey\" constructor bool\n";
  TestSingleWith( bool , 5 , 6 , 7 , 1);
}

TESTMAIN(test_generic_image_view);
