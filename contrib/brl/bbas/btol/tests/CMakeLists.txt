# brl/bbas/btol/tests/CMakeLists.txt

ADD_EXECUTABLE( btol_test_all
  test_driver.cxx
  test_face_algs.cxx
)

TARGET_LINK_LIBRARIES( btol_test_all btol vtol vsol vnl testlib )

ADD_TEST(btol_test_face_algs  btol_test_all test_face_algs)

ADD_EXECUTABLE( btol_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( btol_test_include btol )
