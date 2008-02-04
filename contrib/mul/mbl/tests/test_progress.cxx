// This is mul/mbl/tests/test_progress.cxx
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <mbl/mbl_progress_callback.h>
#include <testlib/testlib_test.h>


static double last_reported_progress=-0.01;
static bool monotonic=true;
static bool below_zero=false;
void mbl_test_progress_callback(double d)
{
  vcl_cout << d;
  if (d<=last_reported_progress)
  {
    monotonic=false;
    vcl_cout << " NOT UP";
  }
  if (d<0) below_zero = true;
  last_reported_progress = d;
  vcl_cout << vcl_endl;
}

void test_progress()
{
  vcl_cout << "*******************************\n"
           << " Testing mbl_progress_callback\n"
           << "*******************************\n";

  mbl_progress_callback progress(&mbl_test_progress_callback);
  
  
  progress.set_estimated_iterations("L0", 3, "");
  
  vcl_cout << "inc L0" << vcl_endl;
  vcl_cout << vcl_setprecision(16);
  progress.increment_progress("L0");
  progress.set_estimated_iterations("L1a", 9, "");
  for (unsigned i=0; i<5; ++i)
  {
    vcl_cout << "inc L1a" << vcl_endl;
    progress.increment_progress("L1a");
  }
  TEST_NEAR("Progress is near middle", last_reported_progress, 0.375, 0.05);
  progress.end_progress("L1a");
  
  vcl_cout << "inc L0" << vcl_endl;
  progress.increment_progress("L0");
  progress.set_estimated_iterations("L1b", 10, "");
  for (unsigned i=0; i<15; ++i)
  {
    vcl_cout << "inc L1b" << vcl_endl;
    progress.increment_progress("L1b");
  }
  progress.end_progress("L1b");

  vcl_cout << "inc L0" << vcl_endl;
  progress.increment_progress("L0");
  progress.end_progress("L0");

  TEST ("Progress always increased", monotonic, true);
  TEST ("Progress within limits", !below_zero && last_reported_progress <= 1.0, true);

  TEST_NEAR ("Progress near end", last_reported_progress, 1.0, 0.05);

}

TESTMAIN(test_progress);
