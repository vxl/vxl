# ./vxl/vnl/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vnl_io_test_driver
  # Driver source
  test_driver.cxx

  # The tests
  golden_test_vnl_io.cxx
  test_bignum_io.cxx
  test_diag_matrix_io.cxx
  test_include.cxx
  test_matrix_fixed_io.cxx
  test_matrix_io.cxx
  test_nonlinear_minimizer_io.cxx
  test_rational_io.cxx
  test_real_npolynomial_io.cxx
  test_real_polynomial_io.cxx
  test_sparse_matrix_io.cxx
  test_sym_matrix_io.cxx
  test_vector_fixed_io.cxx
  test_vector_io.cxx
)

TARGET_LINK_LIBRARIES( vnl_io_test_driver vnl_io vsl testlib )

ADD_TEST( vnl_test_include                  vnl_io_test_driver test_include                 )
ADD_TEST( vnl_test_bignum_io                vnl_io_test_driver test_bignum_io               )
ADD_TEST( vnl_test_diag_matrix_io           vnl_io_test_driver test_diag_matrix_io          )
ADD_TEST( vnl_test_matrix_fixed_io          vnl_io_test_driver test_matrix_fixed_io         )
ADD_TEST( vnl_test_matrix_io                vnl_io_test_driver test_matrix_io               )
ADD_TEST( vnl_test_nonlinear_minimizer_io   vnl_io_test_driver test_nonlinear_minimizer_io  )
ADD_TEST( vnl_test_rational_io              vnl_io_test_driver test_rational_io             )
ADD_TEST( vnl_test_real_npolynomial_io      vnl_io_test_driver test_real_npolynomial_io     )
ADD_TEST( vnl_test_real_polynomial_io       vnl_io_test_driver test_real_polynomial_io      )
ADD_TEST( vnl_test_sparse_matrix_io         vnl_io_test_driver test_sparse_matrix_io        )
ADD_TEST( vnl_test_sym_matrix_io            vnl_io_test_driver test_sym_matrix_io           )
ADD_TEST( vnl_test_vector_fixed_io          vnl_io_test_driver test_vector_fixed_io         )
ADD_TEST( vnl_test_vector_io                vnl_io_test_driver test_vector_io               )
ADD_TEST( vnl_golden_test_vnl_io            vnl_io_test_driver golden_test_vnl_io           )
