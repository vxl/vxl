# brl/bbas/imesh/CMakeLists.txt
# indexed mesh library

doxygen_add_library(contrib/brl/bbas/imesh
  DEPENDS core/vgl core/vul
  PACKAGE contrib/brl
  DESCRIPTION "Brown Indexed Mesh Library"
  )

set(imesh_sources
  imesh_vertex.cxx             imesh_vertex.h
  imesh_face.cxx               imesh_face.h
  imesh_half_edge.cxx          imesh_half_edge.h
  imesh_mesh.cxx               imesh_mesh.h

  imesh_fileio.cxx             imesh_fileio.h
  imesh_operations.cxx         imesh_operations.h
  imesh_detection.cxx          imesh_detection.h
)

aux_source_directory(Templates imesh_sources)

add_library(imesh  ${imesh_sources})

target_link_libraries(imesh vgl vul brdb vbl_io vbl)

# Algorithms
add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

install_targets(/lib imesh)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/imesh/ ${imesh_sources})

