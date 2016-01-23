# This is core/vnl/tests/CMakeLists.txt

ADD_EXECUTABLE( vnl_test_all
  # Driver source and utilities
  test_driver.cxx

  # The tests
  test_bignum.cxx
  test_decnum.cxx
  test_complex.cxx
  test_complexify.cxx
  test_inverse.cxx
  test_diag_matrix.cxx
  test_diag_matrix_fixed.cxx
  test_file_matrix.cxx
  test_finite.cxx
  test_math.cxx
  test_na.cxx
  test_matrix.cxx
  test_matrix_exp.cxx
  test_matrix_fixed.cxx
  test_vector_fixed_ref.cxx
  test_matrix_fixed_ref.cxx
  test_numeric_traits.cxx
  test_power.cxx
  test_quaternion.cxx
  test_rational.cxx
  test_polynomial.cxx
  test_real_polynomial.cxx
  test_real_npolynomial.cxx
  test_resize.cxx
  test_rotation_matrix.cxx
  test_sym_matrix.cxx
  test_transpose.cxx
  test_fastops.cxx
  test_vector.cxx
  test_gamma.cxx
  test_random.cxx
  test_alignment.cxx
  test_arithmetic.cxx  test_arithmetic_body.h
  test_hungarian_algorithm.cxx
  test_integrant.cxx
  test_bessel.cxx
  test_crs_index.cxx
  test_sparse_lst_sqr_function.cxx
  test_sparse_matrix.cxx
  test_pow_log.cxx
  test_vnl_index_sort.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
 SET_SOURCE_FILES_PROPERTIES(test_rational.cxx PROPERTIES COMPILE_FLAGS -O1)
 SET_SOURCE_FILES_PROPERTIES(test_bignum.cxx PROPERTIES COMPILE_FLAGS -O3)
 SET_SOURCE_FILES_PROPERTIES(test_finite.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES(vnl_test_all vnl testlib ${CMAKE_THREAD_LIBS})

if(BUILD_CORE_UTILITIES)
  ADD_EXECUTABLE( vnl_test_with_core_utils
    test_driver_with_core_utils.cxx
    test_matlab.cxx
    test_sample.cxx
  )
  TARGET_LINK_LIBRARIES(vnl_test_with_core_utils vnl vpl vul testlib ${CMAKE_THREAD_LIBS})
  ADD_TEST( NAME vnl_test_matlab COMMAND $<TARGET_FILE:vnl_test_with_core_utils> test_matlab                 )
  ADD_TEST( NAME vnl_test_sample COMMAND $<TARGET_FILE:vnl_test_with_core_utils> test_sample                 )
endif()

ADD_EXECUTABLE(vnl_basic_operation_timings basic_operation_timings.cxx)
TARGET_LINK_LIBRARIES(vnl_basic_operation_timings vnl)
ADD_TEST( NAME vnl_basic_operation_timings COMMAND $<TARGET_FILE:vnl_basic_operation_timings>   )

ADD_TEST( NAME vnl_test_bignum COMMAND $<TARGET_FILE:vnl_test_all> test_bignum                 )
ADD_TEST( NAME vnl_test_decnum COMMAND $<TARGET_FILE:vnl_test_all> test_decnum                 )
ADD_TEST( NAME vnl_test_complex COMMAND $<TARGET_FILE:vnl_test_all> test_complex                )
ADD_TEST( NAME vnl_test_complexify COMMAND $<TARGET_FILE:vnl_test_all> test_complexify             )
ADD_TEST( NAME vnl_test_diag_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_diag_matrix            )
ADD_TEST( NAME vnl_test_diag_matrix_fixed COMMAND $<TARGET_FILE:vnl_test_all> test_diag_matrix_fixed      )
ADD_TEST( NAME vnl_test_file_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_file_matrix            )
ADD_TEST( NAME vnl_test_finite COMMAND $<TARGET_FILE:vnl_test_all> test_finite                 )
ADD_TEST( NAME vnl_test_inverse COMMAND $<TARGET_FILE:vnl_test_all> test_inverse                )
ADD_TEST( NAME vnl_test_math COMMAND $<TARGET_FILE:vnl_test_all> test_math                   )
ADD_TEST( NAME vnl_test_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_matrix                 )
ADD_TEST( NAME vnl_test_matrix_exp COMMAND $<TARGET_FILE:vnl_test_all> test_matrix_exp             )
ADD_TEST( NAME vnl_test_matrix_fixed COMMAND $<TARGET_FILE:vnl_test_all> test_matrix_fixed           )
ADD_TEST( NAME vnl_test_vector_fixed_ref COMMAND $<TARGET_FILE:vnl_test_all> test_vector_fixed_ref       )
ADD_TEST( NAME vnl_test_matrix_fixed_ref COMMAND $<TARGET_FILE:vnl_test_all> test_matrix_fixed_ref       )
ADD_TEST( NAME vnl_test_numeric_traits COMMAND $<TARGET_FILE:vnl_test_all> test_numeric_traits         )
ADD_TEST( NAME vnl_test_na COMMAND $<TARGET_FILE:vnl_test_all> test_na                     )
ADD_TEST( NAME vnl_test_random COMMAND $<TARGET_FILE:vnl_test_all> test_random                 )
ADD_TEST( NAME vnl_test_rational COMMAND $<TARGET_FILE:vnl_test_all> test_rational               )
ADD_TEST( NAME vnl_test_polynomial COMMAND $<TARGET_FILE:vnl_test_all> test_polynomial             )
ADD_TEST( NAME vnl_test_real_polynomial COMMAND $<TARGET_FILE:vnl_test_all> test_real_polynomial        )
ADD_TEST( NAME vnl_test_real_npolynomial COMMAND $<TARGET_FILE:vnl_test_all> test_real_npolynomial       )
ADD_TEST( NAME vnl_test_resize COMMAND $<TARGET_FILE:vnl_test_all> test_resize                 )
ADD_TEST( NAME vnl_test_rotation_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_rotation_matrix        )
ADD_TEST( NAME vnl_test_sym_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_sym_matrix             )
ADD_TEST( NAME vnl_test_transpose COMMAND $<TARGET_FILE:vnl_test_all> test_transpose              )
ADD_TEST( NAME vnl_test_fastops COMMAND $<TARGET_FILE:vnl_test_all> test_fastops                )
ADD_TEST( NAME vnl_test_vector COMMAND $<TARGET_FILE:vnl_test_all> test_vector                 )
ADD_TEST( NAME vnl_test_gamma COMMAND $<TARGET_FILE:vnl_test_all> test_gamma                  )
ADD_TEST( NAME vnl_test_arithmetic COMMAND $<TARGET_FILE:vnl_test_all> test_arithmetic             )
ADD_TEST( NAME vnl_test_alignment COMMAND $<TARGET_FILE:vnl_test_all> test_alignment              )
ADD_TEST( NAME vnl_test_hungarian_algorithm COMMAND $<TARGET_FILE:vnl_test_all> test_hungarian_algorithm    )
ADD_TEST( NAME vnl_test_integrant COMMAND $<TARGET_FILE:vnl_test_all> test_integrant              )
ADD_TEST( NAME vnl_test_bessel COMMAND $<TARGET_FILE:vnl_test_all> test_bessel                 )
ADD_TEST( NAME vnl_test_quaternion COMMAND $<TARGET_FILE:vnl_test_all> test_quaternion             )
ADD_TEST( NAME vnl_test_crs_index COMMAND $<TARGET_FILE:vnl_test_all> test_crs_index              )
ADD_TEST( NAME vnl_test_sparse_lst_sqr_function COMMAND $<TARGET_FILE:vnl_test_all> test_sparse_lst_sqr_function)
ADD_TEST( NAME vnl_test_power COMMAND $<TARGET_FILE:vnl_test_all> test_power                  )
ADD_TEST( NAME vnl_test_sparse_matrix COMMAND $<TARGET_FILE:vnl_test_all> test_sparse_matrix          )
ADD_TEST( NAME test_pow_log COMMAND $<TARGET_FILE:vnl_test_all> test_pow_log                )
ADD_TEST( NAME test_vnl_index_sort COMMAND $<TARGET_FILE:vnl_test_all> test_vnl_index_sort         )

ADD_EXECUTABLE(vnl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(vnl_test_include vnl)
ADD_EXECUTABLE(vnl_test_template_include test_template_include.cxx)
TARGET_LINK_LIBRARIES(vnl_test_template_include vnl)
