#include <vcl_iostream.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_real_polynomial.h>
#include <vcl_cmath.h>

void test_real_polynomial()
{
  vnl_real_polynomial f1(3),f2(4);

	for (int i=0;i<=f1.degree();++i) f1[i]=i+1;
	for (int i=0;i<=f2.degree();++i) f2[i]=2*i+1;

  vnl_real_polynomial f3 = f1+f2;
	TEST("f1+f2=f3",vcl_fabs(f1.evaluate(2.7)+f2.evaluate(2.7)-f3.evaluate(2.7))<1e-6, true);

  vnl_real_polynomial f4 = f1-f2;
	TEST("f1-f2=f3",vcl_fabs(f1.evaluate(2.7)-f2.evaluate(2.7)-f4.evaluate(2.7))<1e-6, true);

  vnl_real_polynomial f5 = f1*f2;

	TEST("f1*f2 has correct degree",f5.degree()==(f1.degree()+f2.degree()),true);

	TEST("f1*f2=f3",vcl_fabs(f1.evaluate(2.7)*f2.evaluate(2.7)-f5.evaluate(2.7))<1e-6, true);

  TEST("Integral",vcl_fabs(f1.evaluate_integral(2.0)-(23+1.0/3))<1e-6, true);

	TEST("RMS difference(f1,f2) is zero",vnl_rms_difference(f1,f1,0,1)<1e-8,true);
}

TESTMAIN(test_real_polynomial);
