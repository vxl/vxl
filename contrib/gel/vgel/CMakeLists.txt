#
# gel/vgel/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgel vgel_sources)

SOURCE_FILES(vgel_sources
vgel_kl
vgel_kl_params
#vgel_multi_view_data
)






AUX_SOURCE_DIRECTORY(Templates vgel_sources)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(kl examples)
