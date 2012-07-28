# brl/bbas/bkml/CMakeLists.txt
# KML utilities for Brown University Libraries

SET(bkml_sources
  bkml_write.h bkml_write.cxx
)

ADD_LIBRARY(bkml  ${bkml_sources})

TARGET_LINK_LIBRARIES(bkml vgl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#SUBDIRS(pro)

INSTALL_TARGETS(/lib bkml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bkml ${bkml_sources})