#include "testlib_register.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#if VCL_HAS_EXCEPTIONS
#include <vcl_exception.h>
#endif

#if defined(VCL_VC)
#  include <crtdbg.h>
#  include <windows.h>
#  include <vcl_cstdio.h>

LONG WINAPI vxl_exception_filter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
  // Retrieve exception information
  PVOID ExceptionAddress       = ExceptionInfo->ExceptionRecord->ExceptionAddress;
  DWORD ExceptionCode          = ExceptionInfo->ExceptionRecord->ExceptionCode;
  DWORD* ExceptionInformation  = (DWORD*)ExceptionInfo->ExceptionRecord->ExceptionInformation;

  vcl_fprintf(stderr, "\nTOP-LEVEL EXCEPTION HANDLER\n");
  switch (ExceptionCode)
  {
   case EXCEPTION_ACCESS_VIOLATION:
    vcl_fprintf(stderr, "The instruction at \"0x%.8p\" failed to %s memory at \"0x%.8x\".\n\n",
                ExceptionAddress, ExceptionInformation[0] ? "write to" :"read",
                ExceptionInformation[1]);
    break;

   case EXCEPTION_INT_DIVIDE_BY_ZERO:
    vcl_fprintf(stderr, "The instruction at \"0x%.8p\" caused an exception of integer devision by zero.\n\n",
                ExceptionAddress);
    break;
   default:
    vcl_fprintf(stderr, "The instruction at \"0x%.8p\" caused an unknown exception (exception code: \"0x%.8x\").\n\n",
                ExceptionAddress,
                ExceptionCode);
  }

  // Default action is to abort
  vcl_printf("Execution aborted!\n");
  return EXCEPTION_EXECUTE_HANDLER;
}
#endif // defined(VCL_WIN32)

static vcl_vector<TestMainFunction> testlib_test_func_;
static vcl_vector<vcl_string>       testlib_test_name_;

void
list_test_names( vcl_ostream& ostr )
{
  ostr << "The registered test names are:\n";
  for ( unsigned int i = 0; i < testlib_test_name_.size(); ++i )
    ostr << "   " << testlib_test_name_[i] << '\n';
  ostr << "\nOmitting a test name, or specifying the name \"all\" will run all the tests.\n";
}


void
testlib_enter_stealth_mode()
{
  // check for Dashboard test
  char * env_var1 = vcl_getenv("DART_TEST_FROM_DART");
  char * env_var2 = vcl_getenv("DASHBOARD_TEST_FROM_CTEST");  // DART Client built in CMake
  if ( env_var1 || env_var2 ) {

  // Don't allow DART test to open critical error dialog boxes
#if defined(VCL_VC)
    // No abort or ANSI assertion failure dialog box
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

    // No Windows style ASSERT failure dialog box
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    // No unhandled exceptions dialog box,
    // such as access violation and integer division by zero
    SetUnhandledExceptionFilter( vxl_exception_filter );
#endif //defined(VCL_VC)

    // Disable Borland's floating point exceptions.
  }
}

int testlib_run_test_unit(vcl_vector<vcl_string>::size_type i, int argc, char *argv[])
{
#if VCL_HAS_EXCEPTIONS
  char * env_var1 = vcl_getenv("DART_TEST_FROM_DART");
  char * env_var2 = vcl_getenv("DASHBOARD_TEST_FROM_CTEST");  // DART Client built in CMake
  if ( env_var1 || env_var2 ) {
    try {
      return testlib_test_func_[i]( argc, argv );
    }
    catch (const vcl_exception &e)
    {
      vcl_cerr << "\nTOP-LEVEL EXCEPTION HANDLER                                        **FAILED**\n"
               << e.what() << "\n\n";
      return 1;
    }
  }
// Leave MS structured exceptions to the SE handler.
  else
#endif
  return testlib_test_func_[i]( argc, argv );
}


int
testlib_main( int argc, char* argv[] )
{
  // The caller should already have called register_tests().

  // NOT to produce any dialog windows
  testlib_enter_stealth_mode();

  // Assume the index type for vector<string> and
  // vector<TestMainFunction> are the same.
  typedef vcl_vector<vcl_string>::size_type vec_size_t;

  // Error check.
  if ( testlib_test_func_.size() != testlib_test_name_.size() ) {
    vcl_cerr << "Error: " << testlib_test_func_.size() << " test functions are registered, but "
             << testlib_test_name_.size() << " test names are registered.\n";
    return 1;
  }


  // If a test name is given, try to run it. Otherwise, try to run all
  // the tests. The first argument, if available, is assumed to be a
  // test name. The special test name "all" can be used to run all the tests
  // with the subsequent arguments passed to each test.

  bool test_name_given = argc >= 2;

  if ( test_name_given && vcl_string("all") == argv[1] )
  {
    --argc; ++argv; test_name_given = false;
  }
  if ( test_name_given )
  {
    for ( vec_size_t i = 0; i < testlib_test_name_.size(); ++i )
      if ( testlib_test_name_[i] == argv[1] )
        return testlib_run_test_unit(i, argc-1, argv+1);


    vcl_cerr << "Test " << argv[1] << " not registered.\n";
    list_test_names( vcl_cerr );
  }
  else
  {
    vcl_cout << "No test name provided.  Attempting to run all tests.\n";
    list_test_names( vcl_cout );
    vcl_cout << "If you want to run a single test, specify one of the above on the command line.\n\n" << vcl_flush;

    bool all_pass = true;
    for ( vec_size_t i = 0; i < testlib_test_name_.size(); ++i )
    {
      vcl_cout << "----------------------------------------\n"
               << "Running: " << testlib_test_name_[i] << '\n'
               << "----------------------------------------\n" << vcl_flush;

      int result = testlib_run_test_unit(i, argc, argv);

      vcl_cout << "----------------------------------------\n"
               << testlib_test_name_[i] << " returned " << result << ' '
               << ( result==0 ? "(PASS)" : "(FAIL)" ) << '\n'
               << "----------------------------------------\n" << vcl_flush;
      all_pass &= (result == 0);
    }

    vcl_cout << "\n\nCombined result of " << testlib_test_name_.size() << " tests: "
             << ( all_pass ? "PASS" : "FAIL" ) << vcl_endl;
    return all_pass ? 0 : 1;
  }

  return 1;
}

void testlib_register_test(const vcl_string & name, TestMainFunction func)
{
  testlib_test_func_.push_back(func);
  testlib_test_name_.push_back(name);
}


void testlib_cleanup()
{
  testlib_test_func_.clear();
  testlib_test_func_.clear();
}
