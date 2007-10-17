# This is /v3p/rply/CMakeLists.txt
# library for reading and writing .PLY file

INCLUDE( ${MODULE_PATH}/FindRPLY.cmake )

IF(NOT VXL_USING_NATIVE_RPLY)

SET(rply_sources
  rply.h
  rply.c
  )
 
ADD_LIBRARY(rply ${rply_sources})

ENDIF(NOT VXL_USING_NATIVE_RPLY)