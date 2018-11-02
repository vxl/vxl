// This is core/vul/tests/test_file_iterator.cxx
#include <iostream>
#include <fstream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <testlib/testlib_test.h>

// file iter
#include <vul/vul_file_iterator.h>

static void touch(char const* fn)
{
  std::ofstream f(fn); f << '.';
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

    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 1", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 2", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 3", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 4", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 5", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 1", !f, true);

    TEST("test_file_iterator once 1", found["/tmp/vxltest/a/."], 1);
    TEST("test_file_iterator once 2", found["/tmp/vxltest/a/.."], 1);
    TEST("test_file_iterator once 3", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 4", found["/tmp/vxltest/a/123.txt"], 1);
    TEST("test_file_iterator once 5", found["/tmp/vxltest/a/13.dat"], 1);
  }

  // 1. Check file.*
  {
    std::cout << "Testing /tmp/vxltest/a/123.*" << std::endl;
    vul_file_iterator f("/tmp/vxltest/a/123.*");
    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 6", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 7", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 2", !f, true);

    TEST("test_file_iterator once 6", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 7", found["/tmp/vxltest/a/123.txt"], 1);
  }

  // 2. Check *.ext
  {
    std::cout << "Testing /tmp/vxltest/a/*.dat" << std::endl;
    vul_file_iterator f("/tmp/vxltest/a/*.dat");
    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 8", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 9", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 3", !f, true);

    TEST("test_file_iterator once 8", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 9", found["/tmp/vxltest/a/13.dat"], 1);
  }

  // 3. Check ?.*
  {
    std::cout << "Testing /tmp/vxltest/a/1?3.???" << std::endl;
    vul_file_iterator f("/tmp/vxltest/a/1?3.???");
    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 10", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 11", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 4", !f, true);

    TEST("test_file_iterator once 10", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 11", found["/tmp/vxltest/a/123.txt"], 1);
  }


  // 4. Check 1[23]*.dat
  {
    std::cout << "Testing /tmp/vxltest/a/1[23]*.dat" << std::endl;
    vul_file_iterator f("/tmp/vxltest/a/1[23]*.dat");
    // Assume semantics are such that files appear in order of creation?
    std::map<std::string, int> found;
    TEST("test_file_iterator 12", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 13", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 5", !f, true);

    TEST("test_file_iterator once 12", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 13", found["/tmp/vxltest/a/13.dat"], 1);
  }


#if 0 // "*/..." does not work (yet).  See note in vul_file_iterator.cxx
  {
    vul_file_iterator f("/tmp/vxltest/*/123.dat");
    TEST("test_file_iterator 14", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 15", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 6", !f, true);

    TEST("test_file_iterator once 14", found["/tmp/vxltest/a/123.dat"], 1);
    TEST("test_file_iterator once 15", found["/tmp/vxltest/b/123.dat"], 1);
  }
#endif

  // 4. Remove and check removal
  vpl_unlink("/tmp/vxltest/b/13.dat");
  vpl_unlink("/tmp/vxltest/b/123.txt");
  vpl_unlink("/tmp/vxltest/b/123.dat");
  {
    std::cout << "Testing /tmp/vxltest/b/*" << std::endl;
    vul_file_iterator f("/tmp/vxltest/b/*");
    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 16", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 17", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 7", !f, true);

    TEST("test_file_iterator once 16", found["/tmp/vxltest/b/."], 1);
    TEST("test_file_iterator once 17", found["/tmp/vxltest/b/.."], 1);
  }
  vpl_rmdir("/tmp/vxltest/b");

  vpl_unlink("/tmp/vxltest/a/13.dat");
  vpl_unlink("/tmp/vxltest/a/123.txt");
  vpl_unlink("/tmp/vxltest/a/123.dat");
  vpl_rmdir("/tmp/vxltest/a");
  {
    std::cout << "Testing /tmp/vxltest/*" << std::endl;
    vul_file_iterator f("/tmp/vxltest/*");
    // test for files - don't care about order?
    std::map<std::string, int> found;
    TEST("test_file_iterator 18", found[f()], 0); ++found[f()];
    ++f;
    TEST("test_file_iterator 19", found[f()], 0); ++found[f()];
    ++f;
    TEST("F ran out 8", !f, true);

    TEST("test_file_iterator 18", found["/tmp/vxltest/."], 1);
    TEST("test_file_iterator 19", found["/tmp/vxltest/.."], 1);
  }

  vpl_rmdir("/tmp/vxltest");
}

void test_file_iterator_dos()
{
  // Make a directory structure
  std::string tempdir = vul_temp_filename();
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
    std::cout << "tempdir\\a\\*" << std::endl;
    vul_file_iterator f( (tempdir+"\\a\\*").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 1", f(), std::string(tempdir+"\\a\\."));
    ++f;
    TEST("test_file_iterator 2", f(), std::string(tempdir+"\\a\\.."));
    ++f;
    TEST("test_file_iterator 3", f(), std::string(tempdir+"\\a\\123.dat"));
    ++f;
    TEST("test_file_iterator 4", f(), std::string(tempdir+"\\a\\123.txt"));
    ++f;
    TEST("test_file_iterator 5", f(), std::string(tempdir+"\\a\\13.dat"));
    ++f;
    TEST("F ran out 1", !f, true);
  }

  // 1. Check file.*
  {
    std::cout << "tempdir\\a\\123.*" << std::endl;
    vul_file_iterator f( (tempdir+"\\a\\123.*").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 6", f(), std::string(tempdir+"\\a\\123.dat"));
    ++f;
    TEST("test_file_iterator 7", f(), std::string(tempdir+"\\a\\123.txt"));
    ++f;
    TEST("F ran out 2", !f, true);
  }

  // 2. Check *.ext
  {
    std::cout << "tempdir\\a\\*.dat" << std::endl;
    vul_file_iterator f( (tempdir+"\\a\\*.dat").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 8", f(), std::string(tempdir+"\\a\\123.dat"));
    ++f;
    TEST("test_file_iterator 9", f(), std::string(tempdir+"\\a\\13.dat"));
    ++f;
    TEST("F ran out 3", !f, true);
  }

  // 3. Check ?.*
  {
    std::cout << "tempdir\\a\\1?3.???" << std::endl;
    vul_file_iterator f( (tempdir+"\\a\\1?3.???").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 10", f(), std::string(tempdir+"\\a\\123.dat"));
    ++f;
    TEST("test_file_iterator 11", f(), std::string(tempdir+"\\a\\123.txt"));
    ++f;
    TEST("F ran out 4", !f, true);
  }

  // 4. Check 1[23]*.dat
  {
    std::cout << "tempdir\\a\\1[23]*.dat" << std::endl;
    vul_file_iterator f( (tempdir+"\\a\\1[23]*.dat").c_str() );
    // Assume semantics are such that files appear in order of creation?
    TEST("test_file_iterator 12", f(), std::string(tempdir+"\\a\\123.dat"));
    ++f;
    TEST("test_file_iterator 13", f(), std::string(tempdir+"\\a\\13.dat"));
    ++f;
    TEST("F ran out 5", !f, true);
  }


#if 0 // "*/..." does not work (yet).  See note in vul_file_iterator.cxx
  {
    std::cout << "tempdir\\*\\123.dat" << std::endl;
    vul_file_iterator f( (tempdir+"\\*\\123.dat").c_str() );
    TEST("test_file_iterator 14", f(), (tempdir+"\\a\\123.dat").c_str() );
    ++f;
    TEST("test_file_iterator 15", f(), (tempdir+"\\b\\123.dat").c_str() );
    ++f;
    TEST("F ran out 6", !f, true);
  }
#endif

  // 5. Remove and check removal
  std::cout << "tempdir\\b\\*" << std::endl;
  vpl_unlink( (tempdir+"\\b\\13.dat").c_str() );
  vpl_unlink( (tempdir+"\\b\\123.txt").c_str() );
  vpl_unlink( (tempdir+"\\b\\123.dat").c_str() );
  {
    vul_file_iterator f( (tempdir+"\\b\\*").c_str() );
    TEST("test_file_iterator 16", f(), std::string(tempdir+"\\b\\."));
    ++f;
    TEST("test_file_iterator 17", f(), std::string(tempdir+"\\b\\.."));
    ++f;
    TEST("F ran out 7", !f, true);
  }
  vpl_rmdir( (tempdir+"\\b").c_str() );

  vpl_unlink( (tempdir+"\\a\\13.dat").c_str() );
  vpl_unlink( (tempdir+"\\a\\123.txt").c_str() );
  vpl_unlink( (tempdir+"\\a\\123.dat").c_str() );
  vpl_rmdir( (tempdir+"\\a").c_str() );
  {
  std::cout << "tempdir\\*" << std::endl;
    vul_file_iterator f( (tempdir+"\\*").c_str() );
    TEST("test_file_iterator 18", f(), std::string(tempdir+"\\."));
    ++f;
    TEST("test_file_iterator 19", f(), std::string(tempdir+"\\.."));
    ++f;
    TEST("F ran out 8", !f, true);
  }

  vpl_rmdir( tempdir.c_str() );
}

static void test_file_iterator()
{
#ifndef _WIN32
  test_file_iterator_unix();
#else
  test_file_iterator_dos();
#endif

  std::cout << "Testing non-existent directory" << std::endl;
  vul_file_iterator f("/some/directory/that/does/not/exist/*.blah");
  TEST( "Iterator is empty", bool(f), false );
}

TEST_MAIN(test_file_iterator);
