# ./oxl/oxltest/CMakeLists.txt

SOURCE_FILES( oxltest_sources oxltest.cpp )

IF(WIN32)
  IF (NOT CYGWIN)
    SOURCE_FILES( oxltest_sources StdAfx.cpp )
    SOURCE_FILES( oxltest_sources StdAfx.h )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ADD_EXECUTABLE( oxltest oxltest_sources )
TARGET_LINK_LIBRARIES( oxltest mvl vul vbl vgl vnl_algo )

ADD_TEST(       oxltest oxltest         )
