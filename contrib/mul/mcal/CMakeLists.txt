# mul/mcal/CMakeLists.txt

doxygen_add_library(contrib/mul/mcal
  DEPENDS core/vnl/algo core/vnl contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Component Analysis Library"
)

SET(mcal_sources
  mcal_component_analyzer.cxx       mcal_component_analyzer.h
  mcal_pca.cxx                      mcal_pca.h
  mcal_trivial_ca.cxx               mcal_trivial_ca.h
  mcal_general_ca.cxx               mcal_general_ca.h
  mcal_single_basis_cost.cxx        mcal_single_basis_cost.h
  mcal_sparse_basis_cost.cxx        mcal_sparse_basis_cost.h
  mcal_var_basis_cost.cxx           mcal_var_basis_cost.h
  mcal_add_all_loaders.cxx          mcal_add_all_loaders.h

  mcal_extract_mode.cxx             mcal_extract_mode.h
)
AUX_SOURCE_DIRECTORY(Templates mcal_sources)

ADD_LIBRARY(mcal ${mcal_sources})
TARGET_LINK_LIBRARIES(mcal mbl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

#SUBDIRS(tools)
