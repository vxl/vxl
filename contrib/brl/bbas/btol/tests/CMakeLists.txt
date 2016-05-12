# brl/bbas/btol/tests/CMakeLists.txt

add_executable( btol_test_all
  test_driver.cxx
  test_face_algs.cxx
)
target_link_libraries( btol_test_all btol vtol ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )

add_test( NAME btol_test_face_algs COMMAND $<TARGET_FILE:btol_test_all> test_face_algs)

add_executable( btol_test_include test_include.cxx )
target_link_libraries( btol_test_include btol )
