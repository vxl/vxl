// This is core/vnl/tests/test_cholesky.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_random.h>

#include "test_util.h"

void test_cholesky()
{
  vnl_random rng;
  vnl_matrix<double> A(3,3);
  test_util_fill_random(A.begin(), A.end(), rng);
  A = A * A.transpose();

  vnl_matrix<double> I(3,3);
  I.set_identity();

  {
    vnl_cholesky chol(A);
    vnl_svd<double> svd(A);
    vcl_cout << "cholesky inverse:\n" << chol.inverse() << '\n'
             << "svd inverse:\n" << svd.inverse() << '\n';
    TEST_NEAR("svd.inverse() ~= cholesky.inverse()",
              (chol.inverse() - svd.inverse()).fro_norm(), 0.0, 1e-10);
  }
  {
    vnl_cholesky chol(A);
    TEST_NEAR("Ai * A ~= I", (chol.inverse() * A - I).fro_norm(), 0.0, 1e-11);
    TEST_NEAR("Ai * A ~= I", (A * chol.inverse() - I).fro_norm(), 0.0, 1e-11);
  }
  {
    vnl_cholesky chol(A, vnl_cholesky::estimate_condition);
    TEST_NEAR("Ai * A ~= I", (chol.inverse() * A - I).fro_norm(), 0.0, 1e-11);
    TEST_NEAR("Ai * A ~= I", (A * chol.inverse() - I).fro_norm(), 0.0, 1e-11);
  }
}

TESTMAIN(test_cholesky);
