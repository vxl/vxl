// This is core/vul/tests/test_awk.cxx
#include <sstream>
#include <iostream>
#include <vul/vul_awk.h>
#include <testlib/testlib_test.h>
//:
// \file
// \brief  Rudimentary testing of the vul_awk class
// \author Eric Moyer (Wright State University)
// \date   15 July 2009
// History: built upon the shell of test_regexp, but has no tests in common

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_awk()
{
  //Test empty file
  {
    std::stringstream tmp;
    tmp << "";
    std::stringstream in(tmp.str().c_str());
    vul_awk awk(in);
    TEST("empty:awk on empty file is invalid", !awk, true);
    TEST("empty:awk on creation has line 1", awk.NR(), 1);
  }

  //Test easy file
  {
    std::stringstream tmp;
    tmp << "1a 1b 1c\n"
        << "2a 2b 2c\n"
        << "  3a  3b 3c" << std::endl;
    std::stringstream in(tmp.str().c_str());
    vul_awk awk(in);
    TEST("easy:awk on non-empty file is valid", (bool)awk, true);
    TEST("easy:awk on creation has line 1", awk.NR(), 1);
    char letter[] = "abc";
    for (int line = 0; line < 3; ++line)
    {
      {
        std::stringstream msg;
        msg << "easy:awk has correct # of fields for line:" << (line+1);
        TEST(msg.str().c_str(), awk.NF(), 3);
      }
      for (int i = 0; i < 3; ++i) {
        std::stringstream msg;
        msg << "easy:correct field content line:" << (line+1)
            << " field: " << i;
        std::stringstream expected;
        expected << (line+1) << letter[i];
        TEST(msg.str().c_str(), expected.str(), awk[i]);
      }
      ++awk;
      {
        std::stringstream msg;
        msg << "easy:awk has correct line number for line:" << (line+2);
        TEST(msg.str().c_str(), awk.NR(), line+2);
      }
    }
    TEST("easy:awk at end of file is invalid", !awk, true);
  }

  //Test commented file with different number of fields per line
  {
    std::stringstream tmp;
    tmp << "#Commented test file\n"
        << "2a 2b 2c #\n"
        << "3a 3b 3c 3d 3e #3f\n"
        << "#Another comment in the middle\n"
        << "  5a # Just one field and comment\n"
        << "  6a\n"
        << "#Last line is a comment" << std::endl;
    std::stringstream in(tmp.str().c_str());
    vul_awk awk(in,vul_awk::strip_comments);
    TEST("comment:awk on non-empty file is valid", (bool)awk, true);
    TEST("comment:awk on creation has line 2", awk.NR(), 2);
    char letter[] = "abcdef";
    //lineNum[numIncrements] has the expected line in the file number
    //after that number of increments have occurred
    int lineNum[] = {2,3,5,6,8};
    //numFields[line] as the number of fields in that line
    int numFields[] = {-1,-1,3,5,-1,1,1,-1};
    for (int numIncrements = 0; numIncrements < 4; ++numIncrements){
      int line = lineNum[numIncrements];
      {
        std::stringstream msg;
        msg << "comment:awk has correct # of fields for line:" << line;
        TEST(msg.str().c_str(), awk.NF(), numFields[line]);
      }
      for (int i = 0; i < numFields[line]; ++i){
        std::stringstream msg;
        msg << "comment:correct field content line:" << line << " field: " << i;
        std::stringstream expected;
        expected << line << letter[i];
        TEST(msg.str().c_str(), expected.str(), awk[i]);
      }
      ++awk;
      {
        std::stringstream msg;
        msg << "comment:awk has correct line number after "
            << (numIncrements+1) << " increments.";
        TEST(msg.str().c_str(), awk.NR(), lineNum[numIncrements+1]);
      }
    }
    TEST("comment:awk at end of file is invalid", !awk, true);
  }
}

TEST_MAIN(test_awk);
