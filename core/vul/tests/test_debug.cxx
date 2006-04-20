// This is core/vul/tests/test_debug.cxx

//:
// \file
// \brief Tests core dumping etc.
// \author Ian Scott
//

#include <vcl_iostream.h>
#include <vcl_new.h>
#include <vul/vul_debug.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>


#include <testlib/testlib_test.h>


//=======================================================================
int main()
{
  vcl_cout <<"\n*******************\n"
           <<  " Testing vul_debug\n"
           <<  "*******************\n\n";

  testlib_test_start("Test Debug");

  const char * filetemplate = "test_core%.3d.dmp";
  {
    vcl_cout << "Test simple forced coredump\n";

    const char * filename = "test_core000.dmp";
    vpl_unlink(filename);

    vul_debug_core_dump(filetemplate);

    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);
  }


#ifdef _WIN32
  {
    vcl_cout << "Test Structured exception coredump\n";

    const char * filename = "test_core001.dmp";
    vpl_unlink(filename);

    vul_debug_set_coredump_and_throw_on_windows_se(filetemplate);
    bool caught_exception=false;
    try
    {
      // force an segmentation violation exception
      if (*static_cast<int *>(0)==0)
        vcl_cout << "*Null is false" << vcl_endl;
      else
        vcl_cout << "*Null is true" << vcl_endl;
    }
    catch (const vul_debug_windows_structured_exception &)
    {
      caught_exception=true;
    }
    TEST("Exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);
  }
#endif

#ifdef VCL_HAS_EXCEPTIONS
  {
    vcl_cout << "Test out-out-memory coredump\n";

    const char * filename = "test_core002.dmp";
    vpl_unlink(filename);

    vul_debug_set_coredump_and_throw_on_out_of_memory(filetemplate);
    bool caught_exception=false;
    try
    {
      vcl_size_t too_much=0;
      too_much -= 1000;
      char * p = new char[too_much];
      p[0] = '\0';
    }
    catch (const vcl_bad_alloc &)
    {
      caught_exception=true;
    }
    TEST("Exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename)>100, true);
  }
#endif

  return testlib_test_summary();
}

