#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vul/vul_test.h>
#include <vul/vul_temp_filename.h>



// file iter
#include <vul/vul_file_iterator.h>
#include <vcl_fstream.h>
#include <vpl/vpl.h>

static void touch(char const* fn)
{
  vcl_ofstream(fn) << ".";
}

void test_file_iterator_unix()
{
  // Make a directory structure
  vpl_mkdir("/tmp/vxltest", 0777);

  vpl_mkdir("/tmp/vxltest/a", 0777);
  touch("/tmp/vxltest/a/123.dat");
  touch("/tmp/vxltest/a/123.txt");
  touch("/tmp/vxltest/a/13.dat");

  vpl_mkdir("/tmp/vxltest/b", 0777);
  touch("/tmp/vxltest/b/123.dat");
  touch("/tmp/vxltest/b/123.txt");
  touch("/tmp/vxltest/b/13.dat");

  // 0. Check "*"
  {
    vul_file_iterator f("/tmp/vxltest/a/*");
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 1", f(), vcl_string("/tmp/vxltest/a/."));
    ++f;
    TEST("test_file_iterator 2", f(), vcl_string("/tmp/vxltest/a/.."));
    ++f;
    TEST("test_file_iterator 3", f(), vcl_string("/tmp/vxltest/a/123.dat"));
    ++f;
    TEST("test_file_iterator 4", f(), vcl_string("/tmp/vxltest/a/123.txt"));
    ++f;
    TEST("test_file_iterator 5", f(), vcl_string("/tmp/vxltest/a/13.dat"));
    ++f;
    TEST("F ran out 1", !f, true);
  }

  // 1. Check file.*
  {
    vul_file_iterator f("/tmp/vxltest/a/123.*");
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 6", f(), vcl_string("/tmp/vxltest/a/123.dat"));
    ++f;
    TEST("test_file_iterator 7", f(), vcl_string("/tmp/vxltest/a/123.txt"));
    ++f;
    TEST("F ran out 2", !f, true);
  }

  // 2. Check *.ext
  {
    vul_file_iterator f("/tmp/vxltest/a/*.dat");
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 8", f(), vcl_string("/tmp/vxltest/a/123.dat"));
    ++f;
    TEST("test_file_iterator 9", f(), vcl_string("/tmp/vxltest/a/13.dat"));
    ++f;
    TEST("F ran out 3", !f, true);
  }

  // 3. Check ?.*
  {
    vul_file_iterator f("/tmp/vxltest/a/1?3.???");
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 10", f(), vcl_string("/tmp/vxltest/a/123.dat"));
    ++f;
    TEST("test_file_iterator 11", f(), vcl_string("/tmp/vxltest/a/123.txt"));
    ++f;
    TEST("F ran out 4", !f, true);
  }

#if 0 // "*/..." does not work (yet).  See note in vul_file_iterator.cxx
  {
    vul_file_iterator f("/tmp/vxltest/*/123.dat");
    TEST("test_file_iterator 12", f(), vcl_string("/tmp/vxltest/a/123.dat"));
    ++f;
    TEST("test_file_iterator 13", f(), vcl_string("/tmp/vxltest/b/123.dat"));
    ++f;
    TEST("F ran out 5", !f, true);
  }
#endif

  // 4. Remove and check removal
  vpl_unlink("/tmp/vxltest/b/13.dat");
  vpl_unlink("/tmp/vxltest/b/123.txt");
  vpl_unlink("/tmp/vxltest/b/123.dat");
  {
    vul_file_iterator f("/tmp/vxltest/b/*");
    TEST("test_file_iterator 14", f(), vcl_string("/tmp/vxltest/b/."));
    ++f;
    TEST("test_file_iterator 15", f(), vcl_string("/tmp/vxltest/b/.."));
    ++f;
    TEST("F ran out 6", !f, true);
  }
  vpl_rmdir("/tmp/vxltest/b");

  vpl_unlink("/tmp/vxltest/a/13.dat");
  vpl_unlink("/tmp/vxltest/a/123.txt");
  vpl_unlink("/tmp/vxltest/a/123.dat");
  vpl_rmdir("/tmp/vxltest/a");
  {
    vul_file_iterator f("/tmp/vxltest/*");
    TEST("test_file_iterator 16", f(), vcl_string("/tmp/vxltest/."));
    ++f;
    TEST("test_file_iterator 17", f(), vcl_string("/tmp/vxltest/.."));
    ++f;
    TEST("F ran out 7", !f, true);
  }

  vpl_rmdir("/tmp/vxltest");
}

void test_file_iterator_dos()
{
  // Make a directory structure
  vcl_string tempdir = vul_temp_filename();
  vpl_mkdir( tempdir.c_str(), 0777);

  vpl_mkdir( (tempdir+"\\a").c_str(), 0777);
  touch( (tempdir+"\\a\\123.dat").c_str() );
  touch( (tempdir+"\\a\\123.txt").c_str() );
  touch( (tempdir+"\\a\\13.dat").c_str() );

  vpl_mkdir( (tempdir+"\\b").c_str(), 0777);
  touch( (tempdir+"\\b\\123.dat").c_str() );
  touch( (tempdir+"\\b\\123.txt").c_str() );
  touch( (tempdir+"\\b\\13.dat").c_str() );

  // 0. Check "*"
  {
    vul_file_iterator f( (tempdir+"\\a\\*").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 1", f(), vcl_string("./."));
    ++f;
    TEST("test_file_iterator 2", f(), vcl_string("./.."));
    ++f;
    TEST("test_file_iterator 3", f(), vcl_string("./123.dat"));
    ++f;
    TEST("test_file_iterator 4", f(), vcl_string("./123.txt"));
    ++f;
    TEST("test_file_iterator 5", f(), vcl_string("./13.dat"));
    ++f;
    TEST("F ran out 1", !f, true);
  }

  // 1. Check file.*
  {
    vul_file_iterator f( (tempdir+"\\a\\123.*").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 6", f(), vcl_string("./123.dat"));
    ++f;
    TEST("test_file_iterator 7", f(), vcl_string("./123.txt"));
    ++f;
    TEST("F ran out 2", !f, true);
  }

  // 2. Check *.ext
  {
    vul_file_iterator f( (tempdir+"\\a\\*.dat").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 8", f(), vcl_string("./123.dat"));
    ++f;
    TEST("test_file_iterator 9", f(), vcl_string("./13.dat"));
    ++f;
    TEST("F ran out 3", !f, true);
  }

  // 3. Check ?.*
  {
    vul_file_iterator f( (tempdir+"\\a\\1?3.???").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 10", f(), vcl_string("./123.dat"));
    ++f;
    TEST("test_file_iterator 11", f(), vcl_string("./123.txt"));
    ++f;
    TEST("F ran out 4", !f, true);
  }

#if 0 // "*/..." does not work (yet).  See note in vul_file_iterator.cxx
  {
    vul_file_iterator f( (tempdir+"\\*\\123.dat").c_str() );
    TEST("test_file_iterator 12", f(), (tempdir+"\\a\\123.dat").c_str() );
    ++f;
    TEST("test_file_iterator 13", f(), (tempdir+"\\b\\123.dat").c_str() );
    ++f;
    TEST("F ran out 5", !f, true);
  }
#endif

  // 4. Remove and check removal
  vpl_unlink( (tempdir+"\\b\\13.dat").c_str() );
  vpl_unlink( (tempdir+"\\b\\123.txt").c_str() );
  vpl_unlink( (tempdir+"\\b\\123.dat").c_str() );
  {
    vul_file_iterator f( (tempdir+"\\b\\*").c_str() );
    TEST("test_file_iterator 14", f(), vcl_string("./."));
    ++f;
    TEST("test_file_iterator 15", f(), vcl_string("./.."));
    ++f;
    TEST("F ran out 6", !f, true);
  }
  vpl_rmdir( (tempdir+"\\b").c_str() );

  vpl_unlink( (tempdir+"\\a\\13.dat").c_str() );
  vpl_unlink( (tempdir+"\\a\\123.txt").c_str() );
  vpl_unlink( (tempdir+"\\a\\123.dat").c_str() );
  vpl_rmdir( (tempdir+"\\a").c_str() );
  {
    vul_file_iterator f( (tempdir+"\\*").c_str() );
    TEST("test_file_iterator 16", f(), vcl_string("./."));
    ++f;
    TEST("test_file_iterator 17", f(), vcl_string("./.."));
    ++f;
    TEST("F ran out 7", !f, true);
  }

  vpl_rmdir( tempdir.c_str() );
}



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

void test_vul_sleep_timer()
// vul_timer, vpl_sleep
{
  vul_timer tic;
  vpl_sleep(1);
  double t = tic.real() / 1000.0;
  
  vcl_cout << "vul_timer: sleep lasted " << t << " seconds, expected 1.0\n";

  TEST("Sleep for between 0.5 and 3 seconds", t> 0.5 && t < 3.0, true);

}


void vul_test_all()
{
#ifndef VCL_WIN32
  test_file_iterator_unix();
#else
  test_file_iterator_dos();
#endif

  test_vul_file();
  test_vul_sleep_timer();
}


TESTMAIN(vul_test_all)
