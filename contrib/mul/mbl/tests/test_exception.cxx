// This is mul/mbl/tests/test_exception.cxx
#include <mbl/mbl_exception.h>
#include <testlib/testlib_test.h>



void test_exception()
{
  vcl_cout << "*************************\n"
           << "  Testing mbl_exception \n"
           << "*************************\n";

#if VCL_HAS_EXCEPTIONS
  {
    bool caught_error = false;
    try
    {
      mbl_exception_error(mbl_exception_abort("This is just a test"));
    }
    catch (const mbl_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
    }
    TEST("Caught Exception 1", caught_error, true);
  }
  {
    bool caught_error = false;
    try
    {
      throw mbl_exception_abort("This is just a test");
    }
    catch (const mbl_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
    }
    TEST("Caught Exception 2", caught_error, true);
  }
#else
  // No exceptions - all we can do is check that mbl_exception_warning doesn't abort.
  mbl_exception_warning(mbl_exception_abort("This is just a test"));
#endif

}

TESTMAIN(test_exception);
