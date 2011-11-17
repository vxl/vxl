# brl/bbas/imesh/algo/CMakeLists.txt
# indexed mesh library algorithms
INCLUDE( ${VXL_CMAKE_DIR}/FindNetlib.cmake )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(imesh_algo_sources
    imesh_transform.cxx          imesh_transform.h
    imesh_intersect.cxx          imesh_intersect.h
    imesh_operations.cxx         imesh_operations.h
    imesh_pca.cxx                imesh_pca.h
    imesh_project.cxx            imesh_project.h
    imesh_detect.cxx             imesh_detect.h
    imesh_kd_tree.cxx            imesh_kd_tree.h      imesh_kd_tree.txx
    imesh_render.cxx             imesh_render.h
    imesh_imls_surface.cxx       imesh_imls_surface.h imesh_imls_surface.txx
   )
AUX_SOURCE_DIRECTORY(Templates imesh_algo_sources)

IF(NETLIB_FOUND)
  INCLUDE_DIRECTORIES( ${NETLIB_INCLUDE_DIR} )
  SET(imesh_algo_sources ${imesh_algo_sources}
      imesh_generate_mesh.cxx     imesh_generate_mesh.h)
ENDIF (NETLIB_FOUND)

ADD_LIBRARY( imesh_algo ${imesh_algo_sources} )

IF(NETLIB_FOUND)

TARGET_LINK_LIBRARIES( imesh_algo imesh vpgl bpgl bpgl_algo vgl_algo vgl vnl_algo vnl vil netlib)
ELSE(NETLIB_FOUND)
TARGET_LINK_LIBRARIES( imesh_algo imesh vpgl vgl_algo vgl vnl_algo vnl vil )
ENDIF(NETLIB_FOUND)


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib imesh_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/imesh/algo/ ${imesh_algo_sources})
