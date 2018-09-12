# mul/mcal/CMakeLists.txt
include_directories( ${CMAKE_BINARY_DIR}/contrib/mul )

doxygen_add_library(contrib/mul/mcal
  DEPENDS core/vnl/algo core/vnl contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Component Analysis Library"
)

set(mcal_sources
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
aux_source_directory(Templates mcal_sources)

vxl_add_library(LIBRARY_NAME mcal LIBRARY_SOURCES ${mcal_sources})
target_link_libraries(mcal mbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(mcal-obj OBJECT ${mcal_sources})
endif()
