# brl/bbas/bmsh3d/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

ADD_EXECUTABLE( bmsh3d_test_all
      test_driver.cxx
      bmsh3d_test_face_geom.cxx
      test_mesh_break_face.cxx
)
TARGET_LINK_LIBRARIES( bmsh3d_test_all bmsh3d bmsh3d_algo testlib)

ADD_TEST( bmsh3d_test_face_geom ${EXECUTABLE_OUTPUT_PATH}/bmsh3d_test_all bmsh3d_test_face_geom )
ADD_TEST( bmsh3d_test_mesh_break_face ${EXECUTABLE_OUTPUT_PATH}/bmsh3d_test_all test_mesh_break_face )

ADD_EXECUTABLE( bmsh3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bmsh3d_test_include bmsh3d )


