#include <vgui/internals/vgui_generic_vil_image_view.h>

#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>

#include <vxl_config.h>

#define TestWith( T )                                                   \
{                                                                       \
  vcl_cout << "Testing pixel type " << #T << '\n';                      \
  {                                                                     \
    vcl_cout << "  from constructor " << #T << '\n';                    \
    vil_image_view< T > img( 5, 6, 7 );                                 \
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
    vil_image_view< T > img( 5, 6, 7 );                                 \
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

void
test_generic_image_view()
{
  // These tests make sure that the data stored in the generic image
  // are the same as that of the image used to create it.

  TestWith( vxl_uint_32 );
  TestWith( vxl_int_32 );
  TestWith( vxl_uint_16 );
  TestWith( vxl_int_16 );
  TestWith( vxl_byte );
  TestWith( vxl_sbyte );
  TestWith( double );
  TestWith( float );
  TestWith( bool );

  TestWith( vil_rgb<vxl_uint_32> );
  TestWith( vil_rgb<vxl_int_32> );
  TestWith( vil_rgb<vxl_uint_16> );
  TestWith( vil_rgb<vxl_int_16> );
  TestWith( vil_rgb<vxl_byte> );
  TestWith( vil_rgb<vxl_sbyte> );
  TestWith( vil_rgb<double> );
  TestWith( vil_rgb<float> );

  TestWith( vil_rgba<vxl_uint_32> );
  TestWith( vil_rgba<vxl_int_32> );
  TestWith( vil_rgba<vxl_uint_16> );
  TestWith( vil_rgba<vxl_int_16> );
  TestWith( vil_rgba<vxl_byte> );
  TestWith( vil_rgba<vxl_sbyte> );
  TestWith( vil_rgba<double> );
  TestWith( vil_rgba<float> );
}

TESTMAIN(test_generic_image_view);

