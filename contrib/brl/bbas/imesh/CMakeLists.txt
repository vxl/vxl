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

vxl_add_library(LIBRARY_NAME imesh LIBRARY_SOURCES  ${imesh_sources})

target_link_libraries(imesh ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul brdb ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl)

# Algorithms
add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()