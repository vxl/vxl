# Additional functions to support vsol
# brl/bbas/bsol/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
SET(bsol_sources
  bsol_hough_line_index.cxx  bsol_hough_line_index.h
)
AUX_SOURCE_DIRECTORY(Templates bsol_sources)

ADD_LIBRARY(bsol ${bsol_sources})

TARGET_LINK_LIBRARIES(bsol vsol vil vnl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
