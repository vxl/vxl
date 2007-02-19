# mul/mcal/CMakeLists.txt

SET(mcal_sources
  mcal_component_analyzer.cxx       mcal_component_analyzer.h
  mcal_pca.cxx                      mcal_pca.h
  mcal_single_basis_cost.cxx        mcal_single_basis_cost.h
  mcal_sparse_basis_cost.cxx        mcal_sparse_basis_cost.h
)
AUX_SOURCE_DIRECTORY(Templates mcal_sources)

ADD_LIBRARY(mcal ${mcal_sources})
TARGET_LINK_LIBRARIES(mcal mbl vimt vnl vnl_algo)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

#SUBDIRS(tools)
