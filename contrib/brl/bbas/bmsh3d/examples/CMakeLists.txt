# This is brcv/dbmsh3d/examples/CMakeLists.txt
# Command-line programs for meshes

include_directories( ${BRL_INCLUDE_DIR} )

add_executable(bmsh3d_example_mesh2ply bmsh3d_mesh2ply.cxx)
target_link_libraries( bmsh3d_example_mesh2ply bmsh3d_algo ${VXL_LIB_PREFIX}vul )

add_executable( bmsh3d_example_triangles example_triangles.cxx )
target_link_libraries( bmsh3d_example_triangles bmsh3d bmsh3d_algo ${VXL_LIB_PREFIX}vgl)
