INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/tbl)

SUBDIRS(vipl vepl )
# Block building of vepl2 since it doesn't compile under MSVC v6 or v7
# SUBDIRS( vepl2)