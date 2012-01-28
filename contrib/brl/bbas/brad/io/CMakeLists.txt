# basic/brad/io/CMakeLists.txt
# Brown radiometry io Library.

SET(brad_io_sources
  brad_io_image_metadata.h  brad_io_image_metadata.cxx
  brad_io_atmospheric_parameters.h brad_io_atmospheric_parameters.cxx
)
AUX_SOURCE_DIRECTORY(Templates brad_io_sources)

ADD_LIBRARY(brad_io  ${brad_io_sources})
TARGET_LINK_LIBRARIES(brad_io brad vnl_io vbl_io vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib brad_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/brad/io ${brad_io_sources})
