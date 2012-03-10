#include <testlib/testlib_test.h>
#include <bapl/bapl_affine_transform.h>
#include <vnl/vnl_double_2.h>

MAIN( test_transform )
{
  START ("affine transform");

  bapl_affine_transform T;
  vnl_double_2 p(13.3, -101.23);

  TEST("Identity Transform",p,T*p);

  SUMMARY();
}
