#include <vcl/vcl_iostream.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_qr.h>

void test_matrix(char const* name, const vnl_matrix<double>& A, double det = 0)
{
  vnl_qr qr(A);

  vcl_string n(name); n+= ": ";
  AssertNear(n+"Q * R residual", (qr.Q() * qr.R() - A).fro_norm());
  Assert(n+"Q * Q = I", (qr.Q().transpose() * qr.Q()).is_identity(1e-12));

  if (det)
    AssertNear(n+ "Determinant", qr.determinant(), det, 1e-10);
} 

// Driver
// extern "C"  bill this won't link for me -- it thinks everything inside
// is extern "C"
void test_qr()
{
  double A_data[] = {
    89,	   21,	  27,
    62,	   71,	   0,
    84,	   13,	  41,
    16,	    9,	   3,
  };
  vnl_matrix<double> A(A_data, 4,3);
  
  test_matrix("A", A);
  test_matrix("AT", A.transpose());

  test_matrix("A-102", A-102);
  test_matrix("AT-12", A.transpose() - 12);

  test_matrix("AA'*1e-3 - 1", A*A.transpose()*1e-3 - 1,  -2.77433958399998);

  double b_data[] = {
    68, 39, 39, 50
  };
  
  vnl_vector<double> b(b_data, 4);
  vnl_qr qr(A);

  vnl_matlab_print(cerr, qr.Q(), "Q");
  vnl_matlab_print(cerr, qr.R(), "R");
  
  vnl_vector<double> x = qr.solve(b);
  
  double res = (A * x - b).magnitude();

  AssertNear("Solve residual", res, 37.8841, 1e-3);

  {
    double S_data[] = {
      89,	   21,	  27,
      62,	   71,	   0,
      84,	   13,	  41,
    };
    vnl_matrix<double> S(S_data, 3,3);
    test_matrix("S", S, 66431);
    test_matrix("S-100", S-100, -79869);
  }

}

TESTMAIN(test_qr);
