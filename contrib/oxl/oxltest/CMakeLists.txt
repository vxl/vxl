# ./oxl/oxltest/CMakeLists.txt

SET( oxltest_sources oxltest.cpp )

IF(WIN32)
  IF (NOT CYGWIN)
    SET( oxltest_sources ${oxltest_sources} StdAfx.cpp )
    SET( oxltest_sources ${oxltest_sources} StdAfx.h )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ADD_EXECUTABLE( oxltest ${oxltest_sources} )
TARGET_LINK_LIBRARIES( oxltest mvl vul vbl vgl vnl_algo )

ADD_TEST( oxl_test oxltest )
