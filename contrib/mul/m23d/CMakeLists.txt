# mul/m23d/CMakeLists.txt

doxygen_add_library(contrib/mul/m23d
  DEPENDS core/vnl/algo core/vnl core/vgl contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's 2D/3D Library"
)

set(m23d_sources
  m23d_rotation_from_ortho_projection.cxx m23d_rotation_from_ortho_projection.h
  m23d_pure_ortho_projection.cxx          m23d_pure_ortho_projection.h
  m23d_set_q_constraint.cxx               m23d_set_q_constraint.h
  m23d_make_ortho_projection.cxx          m23d_make_ortho_projection.h
  m23d_rotation_matrix.cxx                m23d_rotation_matrix.h
  m23d_ortho_rigid_builder.cxx            m23d_ortho_rigid_builder.h
  m23d_ortho_flexible_builder.cxx         m23d_ortho_flexible_builder.h
  m23d_scaled_ortho_projection.cxx        m23d_scaled_ortho_projection.h
  m23d_correction_matrix_error.cxx        m23d_correction_matrix_error.h
  m23d_select_basis_views.cxx             m23d_select_basis_views.h
)
aux_source_directory(Templates m23d_sources)

vxl_add_library(LIBRARY_NAME m23d LIBRARY_SOURCES ${m23d_sources})
target_link_libraries(m23d mbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

#add_subdirectory(tools)
