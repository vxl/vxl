# brl/bbas/bmsh3d/tests/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )

add_executable( bmsh3d_test_all
      test_driver.cxx
      bmsh3d_test_face_geom.cxx
      test_mesh_break_face.cxx
)
target_link_libraries( bmsh3d_test_all bmsh3d bmsh3d_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_test( NAME bmsh3d_test_face_geom COMMAND $<TARGET_FILE:bmsh3d_test_all> bmsh3d_test_face_geom )
add_test( NAME bmsh3d_test_mesh_break_face COMMAND $<TARGET_FILE:bmsh3d_test_all> test_mesh_break_face )

add_executable( bmsh3d_test_include test_include.cxx )
target_link_libraries( bmsh3d_test_include bmsh3d )
