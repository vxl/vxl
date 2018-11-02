// This is core/vul/tests/test_ios_state.cxx
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vul/vul_ios_state.h>


void output_guff(std::ostream &os)
{
  os << "   " << '*' << 4 << "*\n"
     << "   " << 1.23456789 << '*' << std::endl;
}

void test_ios_state()
{
  std::cout << "*****************************\n"
           << " Testing vul_ios_state_saver\n"
           << "*****************************\n";

  std::cout << "ORIGINAL:\n";
  output_guff(std::cout);
  std::cout << "MODIFIED:\n";
  {
    vul_ios_state_saver saver(std::cout);
    std::cout.precision(3);
    std::cout.setf(std::ios::right|std::ios::fixed, std::ios::floatfield);
    std::cout.width(15);
    output_guff(std::cout);
    std::cout << "RESTORED:\n";
  }
  output_guff(std::cout);


  std::ostringstream ss;
  output_guff(ss);
  std::string s1 = ss.str();
  std::string s2;
  {
    vul_ios_state_saver saver(ss);
    ss.precision(3);
    ss.setf(std::ios::right|std::ios::fixed, std::ios::floatfield);
    ss.width(15);
    output_guff(ss);
    s2 = ss.str().substr(s1.size());
  }
  output_guff(ss);
  std::string s3 = ss.str().substr(s1.size()+s2.size());

  TEST("Modified version is different", s1 != s2, true);
  TEST("Restored version is not different", s1 == s3, true);
}

TEST_MAIN(test_ios_state);
