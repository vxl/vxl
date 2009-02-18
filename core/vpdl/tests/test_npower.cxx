#include <testlib/testlib_test.h>
#include <vpdl/vpdl_npower.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>


MAIN( test_npower )
{
  
  // test the 2*pi^n lookup table
  double two_pi_n = 1.0;
  double two_pi = 2.0*vnl_math::pi;
  bool valid = true;
  for(unsigned int n=0; n<15; ++n){
    bool test = (vcl_abs(vpdl_two_pi_power(n) - two_pi_n)/two_pi_n < 1e-14);
    valid = valid && test;
    if(!test)
      vcl_cout << "(2*Pi)^"<<n<<" is " << vpdl_two_pi_power(n) 
               << " should be "<< two_pi_n << " % error is "
               << (vpdl_two_pi_power(n) - two_pi_n)/two_pi_n <<vcl_endl;
    two_pi_n *= two_pi;
  }
  TEST("2*Pi power lookup correct", valid, true); 
  
  
  SUMMARY();
}

