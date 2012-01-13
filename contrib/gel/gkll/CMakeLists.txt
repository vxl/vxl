# gel/gkll/CMakeLists.txt

doxygen_add_library(contrib/gel/gkll
  DEPENDS contrib/gel/vgel
  PACKAGE contrib/gel
  DESCRIPTION "Kanade-Lucas_Tomasi feature tracker"
  )

SET(gkll_sources
 gkll_kl.cxx              gkll_kl.h
 gkll_kl_params.cxx       gkll_kl_params.h
)
AUX_SOURCE_DIRECTORY(Templates gkll_sources)

IF(BUILD_OXL)
SET(gkll_sources ${gkll_sources}
 gkll_multi_view_data.cxx gkll_multi_view_data.h
 gkll_multi_view_data_vertex_sptr.h
)
ENDIF(BUILD_OXL)

ADD_LIBRARY(gkll ${gkll_sources})
INSTALL_TARGETS(/lib gkll)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/gkll ${gkll_sources})

IF(BUILD_OXL)
 INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
 TARGET_LINK_LIBRARIES(gkll mvl)
ENDIF(BUILD_OXL)

TARGET_LINK_LIBRARIES(gkll vgel_kl vtol vil1 vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
