# contrib/brl/bseg/boxm2/volm/io/CMakeLists.txt

SET(boxm2_volm_io_sources
    boxm2_volm_io_locations.h boxm2_volm_io_locations.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_volm_io_sources)

ADD_LIBRARY(boxm2_volm_io ${boxm2_volm_io_sources})
TARGET_LINK_LIBRARIES(boxm2_volm_io boxm2_volm vsl vbl vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
