
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_math.h>

static int num_tests;
static int tests_passed;
static int tests_failed;
static vcl_string test_name;

bool rsdl_close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-6; }



void 
rsdl_tests_start( const vcl_string& s)
{
  num_tests = 0;
  tests_passed = 0;
  tests_failed = 0;
  test_name = s;
  vcl_cout << "==========================================\n"
	   << "Start testing " << s
	   << "\n==========================================\n";
  vcl_cout.flush();
}

void
rsdl_test_begin( const vcl_string& s )
{
  num_tests ++;
  vcl_cout << " Test " << num_tests << ": " << s;
  vcl_cout.flush();
}

void
rsdl_test_perform( bool success )
{
  if ( success ) {
    tests_passed ++ ;
    vcl_cout << "  PASSED\n";
  } else {
    tests_failed++;
    vcl_cout << "**FAILED**\n";
  }
  vcl_cout.flush();
}

bool
rsdl_tests_summary()
{
  vcl_cout << "==========================================\n";
  vcl_cout << test_name << " " << "Test Summary: ";
  if (tests_failed > 0)
    vcl_cout << tests_passed << " tests succeeded, " << tests_failed
	     <<" tests FAILED *****";
  else
    vcl_cout <<"All " << tests_passed << " tests succeeded";
  vcl_cout.flush();
  return tests_failed > 0;
}


#define TEST(s,p) \
 {                   \
   rsdl_test_begin(s); \
   rsdl_test_perform(p); \
 }

 
