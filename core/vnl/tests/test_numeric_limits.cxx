#include <vcl_iostream.h>
#include <vnl/vnl_numeric_limits.h>
#include <testlib/testlib_test.h>

static
void test_if_bool_defined( const bool* )
{
}

static
void test_if_int_defined( const int* )
{
}

static
void test_if_vnl_float_round_style_defined( const vnl_float_round_style* )
{
}

// if this function comiles and links, then all the constants have
// definitions as they should.
static
void test_static_const_definition()
{

#define TEST_TYPE( Type ) \
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_specialized );\
  test_if_int_defined( &vnl_numeric_limits<Type>::digits );\
  test_if_int_defined( &vnl_numeric_limits<Type>::digits10 );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_signed );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_integer );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_exact );\
  test_if_int_defined( &vnl_numeric_limits<Type>::radix );\
  test_if_int_defined( &vnl_numeric_limits<Type>::min_exponent );\
  test_if_int_defined( &vnl_numeric_limits<Type>::min_exponent10 );\
  test_if_int_defined( &vnl_numeric_limits<Type>::max_exponent );\
  test_if_int_defined( &vnl_numeric_limits<Type>::max_exponent10 );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::has_infinity );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::has_quiet_NaN );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::has_signaling_NaN );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::has_denorm );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_iec559 );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_bounded );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::is_modulo );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::traps );\
  test_if_bool_defined( &vnl_numeric_limits<Type>::tinyness_before );\
  test_if_vnl_float_round_style_defined( &vnl_numeric_limits<Type>::round_style )

  TEST_TYPE(int);
  TEST_TYPE(long);
  TEST_TYPE(unsigned long);
  TEST_TYPE(short);
  TEST_TYPE(unsigned short);
  TEST_TYPE(float);
  TEST_TYPE(double);

#undef TEST_TYPE

}


void test_numeric_limits()
{
  // call it to avoid "unsed function" compiler warnings.
  test_static_const_definition();

  vcl_cout << "dmax  = " << vnl_numeric_limits<double>::max() << vcl_endl
           << "dmin  = " << vnl_numeric_limits<double>::min() << vcl_endl
           << "deps  = " << vnl_numeric_limits<double>::epsilon() << vcl_endl
           << "dnmin = " << vnl_numeric_limits<double>::denorm_min() << vcl_endl
           << "dnan  = " << vnl_numeric_limits<double>::quiet_NaN() << vcl_endl
           << "dinf  = " << vnl_numeric_limits<double>::infinity() << vcl_endl
           << "dninf = " << -vnl_numeric_limits<double>::infinity() << vcl_endl

           << "fmax  = " << vnl_numeric_limits<float>::max() << vcl_endl
           << "fmin  = " << vnl_numeric_limits<float>::min() << vcl_endl
           << "feps  = " << vnl_numeric_limits<float>::epsilon() << vcl_endl
           << "fnmin = " << vnl_numeric_limits<float>::denorm_min() << vcl_endl
           << "fnan  = " << vnl_numeric_limits<float>::quiet_NaN() << vcl_endl
           << "finf  = " << vnl_numeric_limits<float>::infinity() << vcl_endl
           << "fninf = " << -vnl_numeric_limits<float>::infinity() << vcl_endl;

  TEST("dmax", vnl_numeric_limits<double>::max() > 1e300, true);
  TEST("dmin", vnl_numeric_limits<double>::min() < 1e-300, true);
  TEST("fmax", vnl_numeric_limits<double>::max() > 1e38, true);
  TEST("fmin", vnl_numeric_limits<double>::min() < 1e-38, true);
}

TESTMAIN(test_numeric_limits);
