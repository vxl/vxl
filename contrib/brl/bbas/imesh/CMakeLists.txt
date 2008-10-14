# brl/bbas/imesh/CMakeLists.txt
# indexed mesh library


SET(imesh_sources
  imesh_vertex.cxx             imesh_vertex.h
  imesh_face.cxx               imesh_face.h
  imesh_half_edge.cxx          imesh_half_edge.h
  imesh_mesh.cxx               imesh_mesh.h

  imesh_fileio.cxx             imesh_fileio.h
  imesh_operations.cxx         imesh_operations.h
  imesh_detection.cxx          imesh_detection.h

)

#AUX_SOURCE_DIRECTORY(Templates imesh_sources)

ADD_LIBRARY(imesh  ${imesh_sources})

TARGET_LINK_LIBRARIES(imesh vgl vul)

# Algorithms
SUBDIRS( algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



