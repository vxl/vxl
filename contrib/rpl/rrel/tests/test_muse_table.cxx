#include <testlib/testlib_test.h>
#include <rrel/rrel_muse_table.h>

static void
test_muse_table()
{
  rrel_muse_table table;

  double ek_new = table.expected_kth( 5, 100 );
  double ek_old = 0.0620850131542;   //  computed from earlier implementation
  TEST_NEAR("expected_kth 5 100", ek_new, ek_old , 1e-6);

  ek_new = table.expected_kth( 5, 250 );
  ek_old = 0.0249689419482;   //  computed from earlier implementation
  TEST_NEAR("expected_kth 5 250", ek_new, ek_old , 1e-6);

  ek_new = table.expected_kth( 18, 100 );
  ek_old = 0.225253407173;   //  computed from earlier implementation
  TEST_NEAR("expected_kth 18 100", ek_new, ek_old , 1e-6);

  double sdk_new = table.standard_dev_kth( 5, 100 );
  double sdk_old = 0.0269994042217;   //  computed from earlier implementation
  TEST_NEAR("standard_dev_kth 5 100", sdk_new, sdk_old , 1e-6);

  sdk_new = table.standard_dev_kth( 75, 90 );
  sdk_old = 0.125990467102;   //  computed from earlier implementation
  TEST_NEAR("standard_dev_kth 75 90", sdk_new, sdk_old , 1e-6);

  double divisor_new = table.muset_divisor( 5, 100 );
  double divisor_old = 0.155162425343;    //  computed from earlier implementation
  TEST_NEAR("divisor 5 100", divisor_new, divisor_old , 1e-6);

  divisor_new = table.muset_divisor( 55, 100 );
  divisor_old = 19.5910236622;    //  computed from earlier implementation
  TEST_NEAR("divisor 55 100", divisor_new, divisor_old , 1e-6);

  double sq_divisor_new = table.muset_sq_divisor( 200, 450 );
  double sq_divisor_old = 22.0039942285;    //  computed from earlier implementation
  TEST_NEAR("sq_divisor 200 450", sq_divisor_new, sq_divisor_old , 1e-6);

  sq_divisor_new = table.muset_sq_divisor( 5, 100 );
  sq_divisor_old = 0.00642924247122;    //  computed from earlier implementation
  TEST_NEAR("sq_divisor 5 100", sq_divisor_new, sq_divisor_old , 1e-6);

  ek_new = table.expected_kth( 75, 90 );
  ek_old = 1.35372487955;    //  computed from earlier implementation
  TEST_NEAR("expected_kth 75 90", ek_new, ek_old , 1e-6);
}

TESTMAIN(test_muse_table);
