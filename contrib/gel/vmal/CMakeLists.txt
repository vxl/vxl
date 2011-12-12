# gel/vmal/CMakeLists.txt

doxygen_add_library(contrib/gel/vmal
  DEPENDS contrib/oxl/osl
  PACKAGE contrib/gel
  DESCRIPTION "Multi-View Line matching"
  )
IF(BUILD_OXL)
SET(vmal_sources
 vmal_dense_matching.cxx    vmal_dense_matching.h
 vmal_homog2d.cxx           vmal_homog2d.h
 vmal_lines_correlation.cxx vmal_lines_correlation.h
 vmal_operators.cxx         vmal_operators.h
 vmal_refine_lines.cxx      vmal_refine_lines.h
 vmal_track_lines.cxx       vmal_track_lines.h
 vmal_convert_vtol.cxx      vmal_convert_vtol.h
)
AUX_SOURCE_DIRECTORY(Templates vmal_sources)


INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
SET(vmal_sources ${vmal_sources}
 vmal_convert_osl.cxx       vmal_convert_osl.h
 vmal_rectifier.cxx         vmal_rectifier.h
 vmal_multi_view_data.cxx   vmal_multi_view_data.h
 vmal_multi_view_data_edge_sptr.h
 vmal_multi_view_data_vertex_sptr.h
)


ADD_LIBRARY(vmal ${vmal_sources})
INSTALL_TARGETS(/lib vmal)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/vmal ${vmal_sources})

IF(BUILD_OXL)
 TARGET_LINK_LIBRARIES(vmal mvl osl vil)
ENDIF(BUILD_OXL)

TARGET_LINK_LIBRARIES(vmal vtol vnl_algo vil1 vnl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
ENDIF(BUILD_OXL)
