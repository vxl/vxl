# gel/vmal/CMakeLists.txt

SET(vmal_sources
 vmal_dense_matching.cxx    vmal_dense_matching.h
 vmal_homog2d.cxx           vmal_homog2d.h
 vmal_kl.cxx                vmal_kl.h
 vmal_kl_params.cxx         vmal_kl_params.h
 vmal_lines_correlation.cxx vmal_lines_correlation.h
 vmal_operators.cxx         vmal_operators.h
 vmal_refine_lines.cxx      vmal_refine_lines.h
 vmal_track_lines.cxx       vmal_track_lines.h
 vmal_convert_vtol.cxx      vmal_convert_vtol.h
IF(BUILD_OXL)
 vmal_convert_osl.cxx       vmal_convert_osl.h
 vmal_rectifier.cxx         vmal_rectifier.h
 vmal_multi_view_data.cxx   vmal_multi_view_data.h
 vmal_multi_view_data_edge_sptr.h
 vmal_multi_view_data_vertex_sptr.h
ENDIF(BUILD_OXL)
)
AUX_SOURCE_DIRECTORY(Templates vmal_sources)

ADD_LIBRARY(vmal ${vmal_sources})

IF(BUILD_OXL)
 INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
 TARGET_LINK_LIBRARIES(vmal mvl osl)
ENDIF(BUILD_OXL)

TARGET_LINK_LIBRARIES(vmal vgel vtol vsol vidl_vil1 vil1 vnl vbl)
