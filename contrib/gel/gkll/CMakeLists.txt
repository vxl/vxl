# gel/gkll/CMakeLists.txt

INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

SET(gkll_sources
 gkll_kl.cxx              gkll_kl.h
 gkll_kl_params.cxx       gkll_kl_params.h
 gkll_multi_view_data.cxx gkll_multi_view_data.h
 gkll_multi_view_data_vertex_sptr.h
)

ADD_LIBRARY(gkll ${gkll_sources})

TARGET_LINK_LIBRARIES(gkll vgel vtol mvl vidl_vil1 vil1 vbl)
