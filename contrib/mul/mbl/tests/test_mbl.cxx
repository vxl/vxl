// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <mbl/tests/test_mz_random.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_mbl()
{
  test_mz_random();
}


TESTMAIN(run_test_mbl);
