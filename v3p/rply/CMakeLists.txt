# This is /v3p/rply/CMakeLists.txt
# library for reading and writing .PLY file

set(rply_sources
  rply.h
  rply.c
  )

add_library(rply ${rply_sources})

#install the .h .txx and libs
install_targets(/lib rply)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/rply/brl/ ${rply_sources})
