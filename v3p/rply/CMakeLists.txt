# This is /v3p/rply/CMakeLists.txt
# library for reading and writing .PLY file

SET(rply_sources
  rply.h
  rply.c
  )
 
ADD_LIBRARY(rply ${rply_sources})
