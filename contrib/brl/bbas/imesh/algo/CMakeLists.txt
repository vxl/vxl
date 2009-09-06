# brl/bbas/imesh/algo/CMakeLists.txt
# indexed mesh library algorithms

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET( imesh_algo_sources
  imesh_transform.cxx          imesh_transform.h
  imesh_intersect.cxx          imesh_intersect.h
  imesh_operations.cxx         imesh_operations.h
  imesh_pca.cxx                imesh_pca.h
  imesh_project.cxx            imesh_project.h
  imesh_detect.cxx             imesh_detect.h
  imesh_kd_tree.cxx            imesh_kd_tree.h      imesh_kd_tree.txx
                               imesh_render.h
  imesh_imls_surface.cxx       imesh_imls_surface.h imesh_imls_surface.txx
)
AUX_SOURCE_DIRECTORY(Templates imesh_algo_sources)

ADD_LIBRARY( imesh_algo ${imesh_algo_sources} )

TARGET_LINK_LIBRARIES( imesh_algo imesh vgl_algo vpgl vil)
