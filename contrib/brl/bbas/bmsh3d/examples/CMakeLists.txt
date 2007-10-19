# This is brcv/dbmsh3d/examples/CMakeLists.txt
# Command-line programs for meshes
# 

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE(bmsh3d_example_mesh2ply bmsh3d_mesh2ply.cxx)
TARGET_LINK_LIBRARIES( bmsh3d_example_mesh2ply bmsh3d_algo vul )

ADD_EXECUTABLE( bmsh3d_example_triangles example_triangles.cxx )
TARGET_LINK_LIBRARIES( bmsh3d_example_triangles bmsh3d bmsh3d_algo)
