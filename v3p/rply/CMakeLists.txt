# This is /v3p/rply/CMakeLists.txt
# library for reading and writing .PLY file

set(rply_sources
  rply.h
  rply.c
  )

vxl_add_library(LIBRARY_NAME rply LIBRARY_SOURCES ${rply_sources})
