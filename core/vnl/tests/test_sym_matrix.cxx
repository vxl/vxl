#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_sym_matrix.h>

void test_int ()
{
  vcl_cout << "*****************************" << vcl_endl;
  vcl_cout << "Testing Symmetric Matrix<int>" << vcl_endl;
  vcl_cout << "*****************************" << vcl_endl;
  vnl_sym_matrix<int> sm1(2);
  TEST ("vnl_sym_matrix<int> m1(2)", (sm1.rows()==2 && sm1.columns()==2), true);
  vnl_sym_matrix<int> sm2(2,2);
  TEST ("vnl_sym_matrix<int> sm2(2,2)",
        (sm2(0,0)==2 && sm2(0,1)==2 && sm2(1,0)==2 && sm2(1,1)==2), true);
  const vnl_matrix<int> ma1(2, 2, 3);
  const vnl_sym_matrix<int> sm3(ma1);
  TEST ("(const vnl_sum_matrix) sm3",
        (sm3(0,0)==3 && sm3(0,1)==3 && sm3(1,0)==3 && sm3(1,1)==3), true);
  vcl_cout << "sm3 \n" << sm3 <<vcl_endl;

  int td[] = {1, 2, 3};
  vnl_sym_matrix<int> sm4(td, 2);
  vcl_cout << "sm4 \n" << sm4 << vcl_endl;
  vnl_matrix<int>  ma2 = sm4.as_matrix();
  TEST ("(const vnl_matrix) ma2", 
    (ma2(0,0)==1 && ma2(0,1)==2 && ma2(1,0)==2 && ma2(1,1)==3), true);
  vcl_cout << "ma2 \n" << ma2 << vcl_endl;
}



void test_matrix()
{
  test_int ();
}

TESTMAIN(test_matrix);
