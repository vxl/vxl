# gel/vmal/CMakeLists.txt

INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

SET(vmal_sources
 vmal_convert.cxx           vmal_convert.h
 vmal_dense_matching.cxx    vmal_dense_matching.h
 vmal_homog2d.cxx           vmal_homog2d.h
 vmal_kl.cxx                vmal_kl.h
 vmal_kl_params.cxx         vmal_kl_params.h
 vmal_lines_correlation.cxx vmal_lines_correlation.h
 vmal_multi_view_data.cxx   vmal_multi_view_data.h
 vmal_operators.cxx         vmal_operators.h
 vmal_rectifier.cxx         vmal_rectifier.h
 vmal_refine_lines.cxx      vmal_refine_lines.h
 vmal_track_lines.cxx       vmal_track_lines.h
 vmal_multi_view_data_edge_sptr.h
 vmal_multi_view_data_vertex_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates vmal_sources)

ADD_LIBRARY(vmal ${vmal_sources})

TARGET_LINK_LIBRARIES(vmal vgel vtol vsol osl mvl vidl_vil1 vil1 vnl vbl)
