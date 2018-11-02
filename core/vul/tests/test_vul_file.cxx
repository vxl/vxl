// This is core/vul/tests/test_vul_file.cxx
#include <fstream>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>


void test_vul_file()
{
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
  std::string rootdir = vul_file::get_cwd();

  vpl_rmdir((rootdir+"/test_make_dir_path/a/b").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path/a").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path").c_str());

  TEST("Directory doesn't exist", vul_file::exists(rootdir+"/test_make_dir_path"), false);
  TEST("make_directory_path", vul_file::make_directory_path(rootdir+"/test_make_dir_path/a/b"), true);
  TEST("Directory does exist", vul_file::is_directory(rootdir+"/test_make_dir_path/a/b"), true);

  vpl_rmdir((rootdir+"/test_make_dir_path/a/b").c_str());
  {
    std::ofstream x((rootdir+"/test_make_dir_path/a/b").c_str()); x << ' ';
  }
  TEST("File exists ...", vul_file::exists(rootdir+"/test_make_dir_path/a/b"), true);
  TEST("... but isn't a directory", vul_file::is_directory(rootdir+"/test_make_dir_path/a/b"), false);

  vpl_unlink((rootdir+"/test_make_dir_path/a/b").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path/a").c_str());
  vpl_rmdir((rootdir+"/test_make_dir_path").c_str());
  TEST("Directory doesn't exist", vul_file::exists(rootdir+"/test_make_dir_path"), false);
  }
#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
  {
  // vul_file::basename
  TEST("wide char basename 1", vul_file::basename(L"fred��.txt"), L"fred��.txt");
  TEST("wide char basename 2", vul_file::basename(L"/awf/fred��.txt"), L"fred��.txt");
  TEST("wide char basename 3", vul_file::basename(L"��fred.txt", L".txt"), L"��fred");
  TEST("wide char basename 4", vul_file::basename(L"/awf/fred.txt", L".txt"), L"fred");
  TEST("wide char basename 5", vul_file::basename(L"t", L".txt"), L"t");
  TEST("wide char basename 6", vul_file::basename(L".txt", L".txt"), L"");
  TEST("wide char basename 7", vul_file::basename(L"��.txt", L".txt"), L"��");
  TEST("wide char basename 8", vul_file::basename(L".ttt", L".txt"), L".ttt");
  TEST("wide char basename 9", vul_file::basename(L"/awf/t", L".txt"), L"t");

  // vul_file::dirname
  TEST("wide char dirname 1", vul_file::dirname(L"fred.txt"), L".");
  TEST("wide char dirname 2", vul_file::dirname(L"/��wf/fred.txt"), L"/��wf");

  // vul_file::make_directory_path
  std::wstring rootdir = vul_file::get_cwd((wchar_t*)0);

  _wrmdir((rootdir+L"/test_make_dir_path/��/b").c_str());
  _wrmdir((rootdir+L"/test_make_dir_path/��").c_str());
  _wrmdir((rootdir+L"/test_make_dir_path").c_str());

  TEST("Directory doesn't exist", vul_file::exists(rootdir+L"/test_make_dir_path"), false);
  TEST("wide char make_directory_path", vul_file::make_directory_path(rootdir+L"/test_make_dir_path/��/b"), true);
  TEST("Directory does exist", vul_file::is_directory(rootdir+L"/test_make_dir_path/��/b"), true);

  _wrmdir((rootdir+L"/test_make_dir_path/��/b").c_str());
  {
    std::ofstream x((rootdir+L"/test_make_dir_path/��/b").c_str()); x << ' ';
  }
  TEST("File exists ...", vul_file::exists(rootdir+L"/test_make_dir_path/��/b"), true);
  TEST("... but isn't a directory", vul_file::is_directory(rootdir+L"/test_make_dir_path/��/b"), false);

  _wunlink((rootdir+L"/test_make_dir_path/��/b").c_str());
  _wrmdir((rootdir+L"/test_make_dir_path/��").c_str());
  _wrmdir((rootdir+L"/test_make_dir_path").c_str());
  TEST("Directory doesn't exist", vul_file::exists(rootdir+L"/test_make_dir_path"), false);
  }
#endif // defined(_WIN32) && VXL_USE_WIN_WCHAR_T
}

TEST_MAIN(test_vul_file);
