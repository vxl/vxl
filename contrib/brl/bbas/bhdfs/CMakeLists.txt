# VXL classes to interact with hadoop libhdfs - a C API for hadoop file system
#
# A note to run executables that will use libdhfs, it requires the environment variable CLASSPATH to be set
# for that one must get the classpath from hadoop:
# TMP=$(/usr/local/hadoop/bin/hadoop classpath)
# and then edit /etc/environment to contain the following:
# CLASSPATH=$CLASSPATH:$TMP
#  e.g. echo $CLASSPATH >> /etc/environment
# 
#
#
INCLUDE( ${BRL_CMAKE_DIR}/FindHadoop.cmake )

IF(HDFS_FOUND)
  ADD_DEFINITIONS(-DHAS_HDFS=1)
  INCLUDE_DIRECTORIES( ${HDFS_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  SET(bhdfs_sources
      bhdfs_manager.h bhdfs_manager.cxx
      bhdfs_fstream.h bhdfs_fstream.cxx
      bhdfs_vil_stream.h bhdfs_vil_stream.cxx
      bhdfs_vil_save.h bhdfs_vil_save.cxx
      bhdfs_vil_load.h bhdfs_vil_load.cxx
  )
  AUX_SOURCE_DIRECTORY(Templates bhdfs_sources)
  ADD_LIBRARY(bhdfs ${bhdfs_sources} )

  TARGET_LINK_LIBRARIES(bhdfs ${HDFS_LIBRARIES})
  
  IF(JAVA_AWT_LIBRARY)
    GET_FILENAME_COMPONENT(JAVA_LIB_DIR ${JAVA_AWT_LIBRARY} PATH)
    IF(EXISTS ${JAVA_LIB_DIR}/xawt)
      LINK_DIRECTORIES(${JAVA_LIB_DIR}/xawt)
    ENDIF(EXISTS ${JAVA_LIB_DIR}/xawt)
    IF(EXISTS ${JAVA_LIB_DIR}/client)
      LINK_DIRECTORIES(${JAVA_LIB_DIR}/client)
    ENDIF(EXISTS ${JAVA_LIB_DIR}/client)
  ENDIF(JAVA_AWT_LIBRARY)
  SET(CMAKE_SKIP_RPATH 0)

  #tests
  SUBDIRS(tests)

  #pro
  SUBDIRS(pro)
ENDIF(HDFS_FOUND)
