# gel/gkll/CMakeLists.txt

SET(gkll_sources
 gkll_kl.cxx              gkll_kl.h
 gkll_kl_params.cxx       gkll_kl_params.h
IF(BUILD_OXL)
 gkll_multi_view_data.cxx gkll_multi_view_data.h
 gkll_multi_view_data_vertex_sptr.h
ENDIF(BUILD_OXL)
)
AUX_SOURCE_DIRECTORY(Templates gkll_sources)

ADD_LIBRARY(gkll ${gkll_sources})

IF(BUILD_OXL)
 INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
 TARGET_LINK_LIBRARIES(gkll mvl)
ENDIF(BUILD_OXL)

TARGET_LINK_LIBRARIES(gkll vgel vtol vidl_vil1 vil1 vbl)
