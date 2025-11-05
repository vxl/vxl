// This is core/vul/tests/test_debug.cxx
#include <iostream>
#include <new>
#include <string>
#include <limits>
#include "testlib/testlib_test.h"
//:
// \file
// \brief Tests core dumping etc.
// \author Ian Scott
//

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vul/vul_debug.h"
#include "vul/vul_file.h"
#include "vul/vul_sprintf.h"
#include "vpl/vpl.h"

//=======================================================================
static void
test_debug()
{
  std::cout << "\n*******************\n"
            << " Testing vul_debug\n"
            << "*******************\n\n";

  const unsigned int pid = vpl_getpid();

  const char * const filetemplate = "test_core%.3d.dmp";
  {
    std::cout << "Test simple forced coredump\n";

    const char * const base_filename = "test_core000.dmp";
    vpl_unlink(base_filename);
    const std::string long_filename = vul_sprintf("%s.%d", base_filename, pid);
    vpl_unlink(long_filename.c_str());

    if (!vul_debug_core_dump(filetemplate))
    {
      std::cout << "Warning: no coredump could be taken\n";
      return;
    }

    TEST("Core dump file exists", vul_file_exists(base_filename) || vul_file_exists(long_filename), true);
    TEST("Core dump file is sensible size", vul_file_size(base_filename) + vul_file_size(long_filename) > 100, true);
  }

#if defined(_WIN32) && THOROUGH_TESTING
  {
    std::cout << "Test Structured exception coredump\n";

    const char * base_filename = "test_core001.dmp";
    vpl_unlink(base_filename);
    std::string long_filename = vul_sprintf("%s.%d", base_filename, pid);
    vpl_unlink(long_filename.c_str());

    vul_debug_set_coredump_and_throw_on_windows_se(filetemplate);
    bool caught_exception = false;
    try
    {
      // force a segmentation violation exception
      if (*static_cast<int *>(0) == 0)
        std::cout << "*Null is false" << std::endl;
      else
        std::cout << "*Null is true" << std::endl;
    }
    catch (const vul_debug_windows_structured_exception &)
    {
      caught_exception = true;
    }
    TEST("Windows SEGV exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(base_filename) || vul_file_exists(long_filename), true);
    TEST("Core dump file is sensible size", vul_file_size(base_filename) + vul_file_size(long_filename) > 100, true);
  }
#endif // defined(_WIN32) && THOROUGH_TESTING
  {
    std::cout << "Test out-of-memory coredump\n";

    const char * const base_filename = "test_core001.dmp";
    vpl_unlink(base_filename);
    const std::string long_filename = vul_sprintf("%s.%d", base_filename, pid);
    vpl_unlink(long_filename.c_str());

    vul_debug_set_coredump_and_throw_on_out_of_memory(filetemplate);

    bool caught_exception = false;
    try
    {
      // NOTE the following line can not be a const or constexpr
      //     otherwise the warning of too large of memory becomes a compiler
      //     error instead of a warning.
      std::size_t too_much = 7;
#ifdef __GNUC__
#  pragma GCC diagnostic push
// disable all warnings here, we really do want to fail at allocation!
#  pragma GCC diagnostic ignored "-Wall"
#endif
      if (!caught_exception)
      {
        too_much = std::numeric_limits<std::size_t>::max();
      }
      /* no diagnostic for this one */
      char * const p = new char[too_much];
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif
      p[0] = '\0';
      delete[] p;
    }
    catch (const std::bad_alloc &)
    {
      caught_exception = true;
    }
    TEST("Out-of-memory exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(base_filename) || vul_file_exists(long_filename), true);
    TEST("Core dump file is sensible size", vul_file_size(base_filename) + vul_file_size(long_filename) > 100, true);
  }
}

TEST_MAIN(test_debug)
