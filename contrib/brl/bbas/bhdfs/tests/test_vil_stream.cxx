//:
// \file
#include <iostream>
#include <testlib/testlib_test.h>
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_vil_stream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test bhdfs vil stream
void test_vil_stream()
{
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"), 0);

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string cur_dir = mins->get_working_dir();

  std::string test_file_name = cur_dir + "/testfile.txt";
  vil_stream* fs = new bhdfs_vil_stream(test_file_name.c_str(), "w");
  TEST("open stream", fs->ok(), true);

  std::string buffer2 = "Hello, World!";
  TEST("write to stream", fs->write(buffer2.c_str(), buffer2.length()), 13);

  fs->ref();
  fs->unref();
  fs = 0;

  vil_stream* fs2 = new bhdfs_vil_stream(test_file_name.c_str(), "r");
  TEST("file size", fs2->file_size(), 13);

  char buf[1000];
  TEST("read from stream", fs2->read(buf, 1000), 13);

  fs2->seek(0);
  TEST("read from stream", fs2->read(buf, 1000), 13);
  TEST("read from stream", fs2->read(buf, 1000), 0);

  fs2->ref();
  fs2->unref();
  fs2 = 0;
}

TESTMAIN(test_vil_stream);
