# brl/bbas/bvrml/CMakeLists.txt
# VRML utilities for Brown University Libraries

SET(bvrml_sources
  bvrml_write.h bvrml_write.cxx
)

ADD_LIBRARY(bvrml  ${bvrml_sources})

TARGET_LINK_LIBRARIES(bvrml vgl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
