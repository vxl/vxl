// This is mul/mbl/tests/test_exception.cxx
#include <iostream>
#include <typeinfo>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_exception.h>
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>


void test_exception()
{
  std::cout << "*************************\n"
           << "  Testing mbl_exception\n"
           << "*************************\n";
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
      std::cout << e.what() << std::endl;
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
      std::cout << e.what() << std::endl;
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
      std::cout << e.what() << std::endl;
      caught_error = true;
      TEST ("Caught expected exception", typeid(e),
            typeid(mbl_exception_os_no_such_file_or_directory));
    }
    TEST("Caught Exception 3", caught_error && !returned, true);
  }
}

TESTMAIN(test_exception);
