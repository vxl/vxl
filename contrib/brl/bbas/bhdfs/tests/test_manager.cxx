//:
// \file
#include <testlib/testlib_test.h>
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

//: Test bhdfs manager object
void test_manager()
{
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(vcl_string("default"), 0);

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  vcl_string cur_dir = mins->get_working_dir();
  vcl_cout << "cur_dir " << cur_dir << vcl_endl;
  TEST("check manager opened FS fine ", mins->ok(), true);
  
  TEST("create dir", mins->create_dir(cur_dir + "/test"), true);
  TEST("exists", mins->exists(cur_dir + "/test"), true);
  
  bhdfs_fstream_sptr fs = new bhdfs_fstream(cur_dir + "/test/test_file.txt", "w");
  TEST("open stream", fs->ok(), true);
  
  vcl_string buffer2 = "Hello, World!"; 
  TEST("write to stream", fs->write(buffer2.c_str(), buffer2.length()), 13);
  TEST("close stream", fs->close(), true);
  
  //: create a local file
  vcl_ofstream ofs("test2.txt", vcl_ios::out);
  ofs << "test\n";
  ofs.close();

  TEST("copy from local", mins->copy_to_hdfs("test2.txt", cur_dir + "/test"), true);
  TEST("copy successful", mins->exists(cur_dir + "/test/test2.txt"), true);
  
  vcl_vector<vcl_string> dir_list;
  TEST("get dir list", mins->get_dir_list(cur_dir + "/test", dir_list), true); 
  for (unsigned i = 0; i < dir_list.size(); i++) 
    vcl_cout << dir_list[i] << vcl_endl;
  TEST("get dir list", dir_list.size() == 2, true);
  TEST("get dir list", dir_list[0].find("test2.txt") != vcl_string::npos, true);
  TEST("copy from hdfs", mins->copy_from_hdfs(dir_list[0], "."), true);
  
  TEST("remove directory", mins->remove_dir(cur_dir + "/test"), true);
  TEST("not exists", mins->exists(cur_dir + "/test"), false);
  
}

TESTMAIN(test_manager);
