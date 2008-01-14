# basic/bsta/io/CMakeLists.txt
# Brown statistics io Library.

SET(bsta_io_sources
  bsta_io_mixture.h
)

AUX_SOURCE_DIRECTORY(Templates bsta_io_sources)

ADD_LIBRARY(bsta_io  ${bsta_io_sources})

SET_TARGET_PROPERTIES(bsta_io PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(bsta_io bsta vnl_io)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

