# gel/vgel/CMakeLists.txt

SET(vgel_sources
 vgel_kl.cxx              vgel_kl.h
 vgel_kl_params.cxx       vgel_kl_params.h
 vgel_multi_view_data.txx vgel_multi_view_data.h
 vgel_multi_view_data_vertex_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates vgel_sources)

ADD_LIBRARY(vgel ${vgel_sources})
TARGET_LINK_LIBRARIES(vgel vgel_kl vtol vil vil1 vidl vidl_vil1 mvl)

INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

SUBDIRS(kl)
IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
