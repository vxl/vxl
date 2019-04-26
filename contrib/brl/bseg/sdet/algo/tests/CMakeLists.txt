# temporary include

add_executable( sdet_algo_test_all
  test_include.cxx
)

target_link_libraries( sdet_algo_test_all sdet_algo ${VXL_LIB_PREFIX}testlib )

add_executable( sdet_algo_test_include test_include.cxx )
target_link_libraries( sdet_algo_test_include sdet_algo )

