// This is core/vul/tests/test_vul_file.cxx
#include <vpl/vpl.h>
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

  // vul_file::make_directory_path
  vcl_string rootdir = vul_file::get_cwd();

  vpl_rmdir((rootdir+"/test_make_dir_path/a/b").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path/a").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path").c_str());

  TEST("Directory doesn't exist", vul_file::is_directory(rootdir+"/test_make_dir_path"), false);
  TEST("make_directory_path", vul_file::make_directory_path(rootdir+"/test_make_dir_path/a/b"), true);
  TEST("Directory does exist", vul_file::is_directory(rootdir+"/test_make_dir_path/a/b"), true);

  vpl_rmdir((rootdir+"/test_make_dir_path/a/b").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path/a").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path").c_str());
  TEST("Directory doesn't exist", vul_file::is_directory(rootdir+"/test_make_dir_path"), false);

}

TESTMAIN(test_vul_file);
