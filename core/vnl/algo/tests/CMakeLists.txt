# This is core/vnl/algo/tests/CMakeLists.txt

INCLUDE( ${VXL_CMAKE_DIR}/FindNetlib.cmake )

# MESSAGE( NETLIB_FOUND " is " ${NETLIB_FOUND} )
IF(NETLIB_FOUND)

  INCLUDE_DIRECTORIES( ${NETLIB_INCLUDE_DIR} )

  ADD_EXECUTABLE( vnl_algo_test_all
    # Driver source and utilities
    test_driver.cxx
    test_util.cxx       test_util.h

    # The tests
    test_algo.cxx
    test_amoeba.cxx
    test_cholesky.cxx
    test_complex_algo.cxx
    test_complex_eigensystem.cxx
    test_convolve.cxx
    test_cpoly_roots.cxx
    test_determinant.cxx
    test_fft.cxx
    test_fft1d.cxx
    test_fft2d.cxx
    test_functions.cxx
    test_generalized_eigensystem.cxx
    test_ldl_cholesky.cxx
    test_levenberg_marquardt.cxx
    test_matrix_update.cxx
    test_minimizers.cxx
    test_powell.cxx
    test_qr.cxx
    test_qsvd.cxx
    test_rank.cxx
    test_real_eigensystem.cxx
    test_rnpoly_roots.cxx
    test_rpoly_roots.cxx
    test_sparse_matrix.cxx
    test_svd.cxx
    test_svd_fixed.cxx
    test_symmetric_eigensystem.cxx
    test_integral.cxx
    test_solve_qp.cxx
    test_sparse_lu.cxx
    test_bracket_minimum.cxx
    test_brent_minimizer.cxx
    test_sparse_lm.cxx
  )

  TARGET_LINK_LIBRARIES( vnl_algo_test_all vnl_algo vul testlib ${CMAKE_THREAD_LIBS} )


  ADD_TEST( vnl_algo_test_algo                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_algo                    )
  ADD_TEST( vnl_algo_test_amoeba                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_amoeba                  )
  ADD_TEST( vnl_algo_test_cholesky                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_cholesky                )
  ADD_TEST( vnl_algo_test_complex_algo             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_complex_algo            )
  ADD_TEST( vnl_algo_test_complex_eigensystem      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_complex_eigensystem     )
  ADD_TEST( vnl_algo_test_convolve                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_convolve                )
  ADD_TEST( vnl_algo_test_cpoly_roots              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_cpoly_roots             )
  ADD_TEST( vnl_algo_test_determinant              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_determinant             )
  ADD_TEST( vnl_algo_test_fft                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_fft                     )
  ADD_TEST( vnl_algo_test_fft1d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_fft1d                   )
  ADD_TEST( vnl_algo_test_fft2d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_fft2d                   )
  ADD_TEST( vnl_algo_test_functions                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_functions               )
  ADD_TEST( vnl_algo_test_generalized_eigensystem  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_generalized_eigensystem )
  ADD_TEST( vnl_algo_test_ldl_cholesky             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_ldl_cholesky            )
  ADD_TEST( vnl_algo_test_levenberg_marquardt      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_levenberg_marquardt     )
  ADD_TEST( vnl_algo_test_matrix_update            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_matrix_update           )
  ADD_TEST( vnl_algo_test_minimizers               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_minimizers              )
  ADD_TEST( vnl_algo_test_powell                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_powell                  )
  ADD_TEST( vnl_algo_test_qr                       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_qr                      )
  ADD_TEST( vnl_algo_test_qsvd                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_qsvd                    )
  ADD_TEST( vnl_algo_test_rank                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_rank                    )
  ADD_TEST( vnl_algo_test_real_eigensystem         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_real_eigensystem        )
  ADD_TEST( vnl_algo_test_rnpoly_roots             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_rnpoly_roots            )
  ADD_TEST( vnl_algo_test_rpoly_roots              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_rpoly_roots             )
  ADD_TEST( vnl_algo_test_integral                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_integral                )
  ADD_TEST( vnl_algo_test_solve_qp                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_solve_qp                )
  ADD_TEST( vnl_algo_test_sparse_lu                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_sparse_lu               )
  ADD_TEST( vnl_algo_test_bracket_minimum          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_bracket_minimum         )
  ADD_TEST( vnl_algo_test_brent_minimizer          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_brent_minimizer         )
  ADD_TEST( vnl_algo_test_sparse_lm                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_sparse_lm               )
  ADD_TEST( vnl_algo_test_sparse_matrix            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_sparse_matrix           )
  ADD_TEST( vnl_algo_test_svd                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_svd                     )
  ADD_TEST( vnl_algo_test_svd_fixed                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_svd_fixed               )
  ADD_TEST( vnl_algo_test_symmetric_eigensystem    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vnl_algo_test_all test_symmetric_eigensystem   )
ENDIF(NETLIB_FOUND)

# GCC 2.95 has problems when compiling test_algo.cxx with "-O2" flag.
# The solution is to turn off optimization for GCC < 3.0
IF(CMAKE_COMPILER_IS_GNUCXX)
  IF ( VNL_COMPILER_IS_GNUCXX_2XX MATCHES "VNL_COMPILER_IS_GNUCXX_2XX")
    EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
    IF(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
      SET ( VNL_COMPILER_IS_GNUCXX_2XX 1 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    ELSE(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
      SET ( VNL_COMPILER_IS_GNUCXX_2XX 0 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    ENDIF(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
  ENDIF ( VNL_COMPILER_IS_GNUCXX_2XX MATCHES "VNL_COMPILER_IS_GNUCXX_2XX")

  IF(VNL_COMPILER_IS_GNUCXX_2XX)
    # We only need to remove "-O2" flag from test_algo.cxx.
    # But it is much easier to do it for all
    FOREACH(var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG
                CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
      STRING(REGEX REPLACE "-O2" "" "${var}" "${${var}}")
    ENDFOREACH(var)
  ENDIF(VNL_COMPILER_IS_GNUCXX_2XX)
ENDIF ( CMAKE_COMPILER_IS_GNUCXX )

ADD_EXECUTABLE( vnl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vnl_algo_test_include vnl_algo )
ADD_EXECUTABLE( vnl_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vnl_algo_test_template_include vnl_algo )
