// This is vxl/vnl/tests/test_qr.cxx
#include <vcl_iostream.h>
#include <vcl_complex.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd.h>

#include "test_util.h"

// void test_matrix(char const* name, const vnl_matrix<double>& A, double det = 0)
// {
//   vnl_qr<double> qr(A);
// 
//   vcl_string n(name); n+= ": ";
//   testlib_test_assert_near(n+"Q * R residual", (qr.Q() * qr.R() - A).fro_norm());
//   testlib_test_assert(n+"Q * Q = I", (qr.Q().transpose() * qr.Q()).is_identity(1e-12));
// 
//   if (det)
//     testlib_test_assert_near(n+ "Determinant", qr.determinant(), det, 1e-10);
// }

extern "C" void test_cholesky() {
  vnl_matrix<double> A(3,3);
  test_util_fill_random(A.begin(), A.end());
  A = A * A.transpose();
  
  vnl_matrix<double> I(3,3); 
  I.set_identity();

  {
    vnl_cholesky chol(A);
    vnl_svd<double> svd(A);
    MATLABPRINT(chol.inverse());
    MATLABPRINT(svd.inverse());
  }

  {
    vnl_cholesky chol(A);
    testlib_test_assert_near("Ai * A - I", (chol.inverse() * A - I).fro_norm());
  }
  {
    vnl_cholesky chol(A, vnl_cholesky::estimate_condition);
    testlib_test_assert_near("Ai * A - I", (chol.inverse() * A - I).fro_norm());
  }
}

TESTMAIN(test_cholesky);
