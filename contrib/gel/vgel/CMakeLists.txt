# gel/vgel/CMakeLists.txt

ADD_LIBRARY(vgel vgel_sources)

SOURCE_FILES(vgel_sources
 vgel_kl.cxx              vgel_kl.h
 vgel_kl_params.cxx       vgel_kl_params.h
 vgel_multi_view_data.txx vgel_multi_view_data.h
 vgel_multi_view_data_vertex_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates vgel_sources)


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(kl)
IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
