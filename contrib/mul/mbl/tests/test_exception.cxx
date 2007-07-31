// This is mul/mbl/tests/test_exception.cxx
#include <mbl/mbl_exception.h>
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>



void test_exception()
{
  vcl_cout << "*************************\n"
           << "  Testing mbl_exception \n"
           << "*************************\n";

#if VCL_HAS_EXCEPTIONS
  {
    bool caught_error = false;
    bool returned = false;
    try
    {
      mbl_exception_error(mbl_exception_abort("This is just a test"));
      returned = true;
    }
    catch (const mbl_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
    }
    TEST("Caught Exception 1", caught_error && !returned, true);
  }
  {
    bool caught_error = false;
    bool returned = false;
    try
    {
      throw mbl_exception_abort("This is just a test");
      returned = true;
    }
    catch (const mbl_exception_abort &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
    }
    TEST("Caught Exception 2", caught_error && !returned, true);
  }

  {
    bool caught_error = false;
    bool returned = false;
    try
    {
      const char *filename="There_is_no_way_this_directory_should_exist/try_to_create_this";
      vul_file::make_directory(filename);
      mbl_exception_throw_os_error(filename);
      returned = true;
    }
    catch (const mbl_exception_os_error &e)
    {
      vcl_cout << e.what() << vcl_endl;
      caught_error = true;
      TEST ("Caught expected exception", typeid(e),
        typeid(mbl_exception_os_no_such_file_or_directory));
    }
    TEST("Caught Exception 3", caught_error && !returned, true);
  }

#else
  // No exceptions - all we can do is check that mbl_exception_warning, etc. doesn't abort.
  mbl_exception_warning(mbl_exception_abort("This is just a test"));
  const char *filename="There_is_no_way_this_directory_should_exist/try_to_create_this";
  vpl_mkdir(filename);
  mbl_exception_throw_os_error(filename);

#endif

}

TESTMAIN(test_exception);
