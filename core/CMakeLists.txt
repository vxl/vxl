# vxl/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

SET(global_sources
 vxl_copyright.h
 vxl_config.h.in
)

# common test executable
SUBDIRS(testlib)

# POSIX layer
SUBDIRS(vpl)

# stream I/O
SUBDIRS(vsl)

# utilities
SUBDIRS(vul)

# "basics" (templates)
SUBDIRS(vbl)

# numerics
SUBDIRS(vnl)

# geometry (including computational geometry)
SUBDIRS(vgl)

# image
SUBDIRS(vil)

# video
SUBDIRS(vidl)

# coordinate systems
SUBDIRS(vcsl)

IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)
