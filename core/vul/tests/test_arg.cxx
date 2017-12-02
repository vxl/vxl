//-*- c++ -*-------------------------------------------------------------------
//
// test_vul_arg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 21 Jan 97
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include <list>
#include <algorithm>
#include <vul/vul_arg.h>
#include <vcl_compiler.h>
#include <testlib/testlib_test.h>

char const * my_argv_1[] =
{
  "progname",
  "f",
  "-int", "3",
  "g",
  "-bool1",
  "-bool1",
  "-bool2",
  "-list1",
  "1:2,10,21:3:25,-1:-2:-7",
  "-list2",
  "1.5,-1.5,8.0",
  "h",
  "i",
  "j",
  "-req", "100",
  VXL_NULLPTR
};

int list1_contents[] = { 1,2,10,21,24,-1,-3,-5,-7 };

double list2_contents[] = { 1.5,-1.5,8.0 };

int count_my_args(char const * const * my_argv)
{
  int c = 0;
  for (; *my_argv; ++my_argv)
    ++c;
  return c;
}

void test_arg()
{
  vul_arg_info_list arglist;

  vul_arg<int> int1(arglist, "-int", "A help string", 1);
  vul_arg<int> int2(arglist, "-int2", "Another help string", 2);
  vul_arg<bool> bool1(arglist, "-bool1", "And another", false);
  vul_arg<bool> bool2(arglist, "-bool2", "And another", true);
  vul_arg<bool> bool3(arglist, "-bool3", "And a final help test just to finish off...", true);
  vul_arg<std::list<int> > list1(arglist, "-list1", "List...");
  vul_arg<char*> filename1(arglist);
  vul_arg<std::vector<double> > list2(arglist, "-list2", "double List...");
  vul_arg<int> required (arglist, "-req", "Another one to declare a required flag", vul_arg<int>::is_required);

  int my_argc = count_my_args(my_argv_1);
  std::cout << "vul_argc = " << my_argc
           << ", int1 = " << int1()
           << ", int2 = " << int2()
           << ", bool1 = " << bool1()
           << ", bool2 = " << bool2()
           << ", bool3 = " << bool3()
           << ", list1 size = " << list1().size()
           << ", list2 size = " << list2().size()
           << ", req = " << required()
           << std::endl;
  char **my_argv = (char**) my_argv_1; // casting away const !!!

  arglist.parse(my_argc, my_argv, true);

  TEST("int1", int1(), 3);
  TEST("int2", int2(), 2);
  TEST("req", required(), 100);
  TEST("filename == f", std::strcmp(filename1(), "f"), 0);

  {
    unsigned true_list_length = sizeof list1_contents / sizeof list1_contents[0];
    std::list<int> l = list1();
    TEST("list1 length", l.size(), true_list_length);
    bool ok = true;
    for (unsigned int i = 0; i < true_list_length; ++i) {
      if (std::find(l.begin(), l.end(), list1_contents[i]) == l.end()) {
        std::cout << "Integer [" << list1_contents[i] << "] not found in list\n";
        ok = false;
      }
    }
    TEST("list1 contents", ok, true);
  }
  {
    unsigned true_list_length = sizeof list2_contents / sizeof list2_contents[0];
    std::vector<double> l = list2();
    TEST("list2 length", l.size(), true_list_length);
    bool ok = true;
    for (unsigned int i = 0; i < true_list_length; ++i) {
      if (std::find(l.begin(), l.end(), list2_contents[i]) == l.end()) {
        std::cout << "Value [" << list2_contents[i] << "] not found in list\n";
        ok = false;
      }
    }
    TEST("list2 contents", ok, true);
  }
}

TEST_MAIN(test_arg);
