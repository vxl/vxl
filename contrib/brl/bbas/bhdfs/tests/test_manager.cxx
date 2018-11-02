//:
// \file
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test bhdfs manager object
void test_manager()
{
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"), 0);

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string cur_dir = mins->get_working_dir();
  std::cout << "cur_dir " << cur_dir << std::endl;
  TEST("check manager opened FS fine ", mins->ok(), true);

  TEST("create dir", mins->create_dir(cur_dir + "/test"), true);
  TEST("exists", mins->exists(cur_dir + "/test"), true);

  bhdfs_fstream_sptr fs = new bhdfs_fstream(cur_dir + "/test/test_file.txt", "w");
  TEST("open stream", fs->ok(), true);

  std::string buffer2 = "Hello, World!";
  TEST("write to stream", fs->write(buffer2.c_str(), buffer2.length()), 13);
  TEST("close stream", fs->close(), true);

  //: create a local file
  std::ofstream ofs("test2.txt", std::ios::out);
  ofs << "test\n";
  ofs.close();

  TEST("copy from local", mins->copy_to_hdfs("test2.txt", cur_dir + "/test"), true);
  TEST("copy successful", mins->exists(cur_dir + "/test/test2.txt"), true);

  std::vector<std::string> dir_list;
  TEST("get dir list", mins->get_dir_list(cur_dir + "/test", dir_list), true);
  for (unsigned i = 0; i < dir_list.size(); i++)
    std::cout << dir_list[i] << std::endl;
  TEST("get dir list", dir_list.size() == 2, true);
  TEST("get dir list", dir_list[0].find("test2.txt") != std::string::npos, true);
  TEST("copy from hdfs", mins->copy_from_hdfs(dir_list[0], "."), true);

  TEST("remove directory", mins->remove_dir(cur_dir + "/test"), true);
  TEST("not exists", mins->exists(cur_dir + "/test"), false);

}

TESTMAIN(test_manager);
