// This is gel/vifa/tests/test_parallel_params.cxx
#include <testlib/testlib_test.h>
#include <vifa/vifa_parallel_params.h>


static void test_parallel_params()
{
  vifa_parallel_params  vpp;

  vpp.print_info();
}


TESTMAIN(test_parallel_params);
