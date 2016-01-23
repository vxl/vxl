# brl/bbas/imesh/algo/CMakeLists.txt
# indexed mesh library algorithms
include( ${VXL_CMAKE_DIR}/FindNetlib.cmake )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(imesh_algo_sources
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
aux_source_directory(Templates imesh_algo_sources)

if(NETLIB_FOUND)
  include_directories( ${NETLIB_INCLUDE_DIR} )
  set(imesh_algo_sources ${imesh_algo_sources}
      imesh_generate_mesh.cxx     imesh_generate_mesh.h)
endif(NETLIB_FOUND)

add_library( imesh_algo ${imesh_algo_sources} )

if(NETLIB_FOUND)

target_link_libraries( imesh_algo imesh vpgl bpgl bpgl_algo vgl_algo vgl vnl_algo vnl vil netlib)
else(NETLIB_FOUND)
target_link_libraries( imesh_algo imesh vpgl vgl_algo vgl vnl_algo vnl vil )
endif(NETLIB_FOUND)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

install_targets(/lib imesh_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/imesh/algo/ ${imesh_algo_sources})
