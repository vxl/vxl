//:
// \file
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
//#include <vcl_compiler.h>

//: Test bhdfs fstream
void test_fstream()
{
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"), 0);

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string cur_dir = mins->get_working_dir();

  std::string test_file_name = cur_dir + "/testfile.txt";
  bhdfs_fstream_sptr fs = new bhdfs_fstream(test_file_name, "w");
  //bhdfs_fstream_sptr fs = new bhdfs_fstream(test_file_name, "a");
  //bhdfs_fstream_sptr fs = new bhdfs_fstream(test_file_name, "r");
  TEST("open stream", fs->ok(), true);

  std::string buffer2 = "Hello, World!";
  TEST("write to stream", fs->write(buffer2.c_str(), buffer2.length()), 13);

  TEST("close stream", fs->close(), true);

  bhdfs_fstream_sptr fs2 = new bhdfs_fstream(test_file_name, "r");
  TEST("file size", fs2->file_size(), 13);

  char buf[1000];
  TEST("read from stream", fs2->read(buf, 1000), 13);

  fs2->seek(0);
  TEST("read from stream", fs2->read(buf, 1000), 13);
  TEST("read from stream", fs2->read(buf, 1000), 0);

  fs2->close();

}

TESTMAIN(test_fstream);
