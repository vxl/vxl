
// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)
#include <vnl/io/tests/test_vector_io.cxx>
#include <vnl/io/tests/test_real_polynomial_io.cxx>
#include <vnl/io/tests/test_matrix_io.cxx>
#include <vnl/io/tests/test_real_npolynomial_io.cxx>
#include <vnl/io/tests/test_diag_matrix_io.cxx>
#include <vnl/io/tests/test_matrix_fixed_io.cxx>
#include <vnl/io/tests/test_vector_fixed_io.cxx>
#include <vnl/io/tests/test_sparse_matrix_io.cxx>
#include <vnl/io/tests/test_nonlinear_minimizer_io.cxx>
#include <vnl/io/tests/test_rational_io.cxx>
#include <vnl/io/tests/test_bignum_io.cxx>
#include <vnl/io/tests/test_sym_matrix_io.cxx>
#include <vnl/io/tests/golden_test_vnl_io.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() {vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_vnl_io()
{
  test_vector_double_io();
  test_real_polynomial_io();
  test_matrix_double_io();
  test_real_npolynomial_io();
  test_diag_matrix_double_io();
  test_matrix_fixed_double_2_2_io();
  test_vector_fixed_double_3_io();
  test_sparse_matrix_double_io();
  test_nonlinear_minimizer_io();
  test_rational_io();
  test_bignum_io();
  test_sym_matrix_io();
  golden_test_vnl_io();
}

TESTMAIN(run_test_vnl_io);
