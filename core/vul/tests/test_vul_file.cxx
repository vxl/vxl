// This is core/vul/tests/test_vul_file.cxx
#include <vul/vul_file.h>
#include <testlib/testlib_test.h>


void test_vul_file()
{
  // vul_file::basename
  TEST("basename 1", vul_file::basename("fred.txt"), "fred.txt");
  TEST("basename 2", vul_file::basename("/awf/fred.txt"), "fred.txt");
  TEST("basename 3", vul_file::basename("fred.txt", ".txt"), "fred");
  TEST("basename 4", vul_file::basename("/awf/fred.txt", ".txt"), "fred");
  TEST("basename 5", vul_file::basename("t", ".txt"), "t");
  TEST("basename 6", vul_file::basename(".txt", ".txt"), "");
  TEST("basename 7", vul_file::basename("t.txt", ".txt"), "t");
  TEST("basename 8", vul_file::basename(".ttt", ".txt"), ".ttt");
  TEST("basename 9", vul_file::basename("/awf/t", ".txt"), "t");

  // vul_file::dirname
  TEST("dirname 1", vul_file::dirname("fred.txt"), ".");
  TEST("dirname 2", vul_file::dirname("/awf/fred.txt"), "/awf");
}

TESTMAIN(test_vul_file);
