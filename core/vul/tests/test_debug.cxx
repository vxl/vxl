// This is core/vul/tests/test_debug.cxx

//:
// \file
// \brief Tests core dumping etc.
// \author Ian Scott
//

#include <vcl_iostream.h>
#include <vul/vul_debug.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>


#include <testlib/testlib_test.h>


//=======================================================================
void test_debug()
{
  vcl_cout <<"\n*******************\n"
           <<  " Testing vul_debug\n"
           <<  "*******************\n\n";

  const char * filename = "test_core.dmp";
  vpl_unlink(filename);

  vul_debug_core_dump(filename);

  TEST("Core dump file exists", vul_file_exists(filename), true);
  TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);

}

TESTMAIN(test_debug);
