// This is core/vul/tests/test_ios_state.cxx
#include <vcl_iostream.h>
// not used? #include <vcl_iomanip.h>
#include <testlib/testlib_test.h>
#include <vul/vul_ios_state.h>


void output_guff(vcl_ostream &os)
{
  os << "   " << '*' << 4 << "*\n"
     << "   " << 1.23456789 << '*' << vcl_endl;
}

void test_ios_state()
{
  vcl_cout << "*****************************\n"
           << " Testing vul_ios_state_saver\n"
           << "*****************************\n";

  vcl_cout << "ORIGINAL:\n";
  output_guff(vcl_cout);
  vcl_cout << "MODIFIED:\n";
  {
    vul_ios_state_saver saver(vcl_cout);
    vcl_cout.precision(3);
    vcl_cout.setf(vcl_ios_fixed|vcl_ios_right);
    vcl_cout.width(15);
    output_guff(vcl_cout);
    vcl_cout << "RESTORED:\n";
  }
  output_guff(vcl_cout);


  vcl_ostringstream ss;
  output_guff(ss);
  vcl_string s1 = ss.str();
  vcl_string s2;
  {
    vul_ios_state_saver saver(ss);
    ss.precision(3);
    ss.setf(vcl_ios_fixed|vcl_ios_right);
    ss.width(15);
    output_guff(ss);
    s2 = ss.str().substr(s1.size());
  }
  output_guff(ss);
  vcl_string s3 = ss.str().substr(s1.size()+s2.size());

  TEST("Modified version is different", s1 != s2, true);
  TEST("Restored version is not different", s1 == s3, true);
}

//TESTMAIN(test_ios_state);
int test_ios_state(int, char*[])
{
  testlib_test_start("test_ios_state");

  test_ios_state();

  return testlib_test_summary();
}
