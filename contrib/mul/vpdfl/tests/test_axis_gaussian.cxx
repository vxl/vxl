#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vpdfl/vpdfl_axis_gaussian.h>

void test_axis_gaussian()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing vpdfl_axis_gaussian " << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

//  vsl_add_to_binary_loader(vpdfl_axis_gaussian());

//  TEST("lrand32",mz_random.lrand32()==3501493769,true);

}

TESTMAIN(test_axis_gaussian);
