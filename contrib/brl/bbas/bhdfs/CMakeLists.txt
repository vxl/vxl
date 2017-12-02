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
include( ${BRL_CMAKE_DIR}/FindHadoop.cmake )

if(HDFS_FOUND)
  add_definitions(-DHAS_HDFS=1)
  include_directories( ${HDFS_INCLUDE_DIRS} )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )

  set(bhdfs_sources
      bhdfs_manager.h bhdfs_manager.cxx
      bhdfs_fstream.h bhdfs_fstream.cxx
      bhdfs_vil_stream.h bhdfs_vil_stream.cxx
      bhdfs_vil_save.h bhdfs_vil_save.cxx
      bhdfs_vil_load.h bhdfs_vil_load.cxx
  )
  aux_source_directory(Templates bhdfs_sources)
  vxl_add_library(LIBRARY_NAME bhdfs LIBRARY_SOURCES ${bhdfs_sources} )

  target_link_libraries(bhdfs ${HDFS_LIBRARIES})

  if(JAVA_AWT_LIBRARY)
    get_filename_component(JAVA_LIB_DIR ${JAVA_AWT_LIBRARY} PATH)
    if(EXISTS ${JAVA_LIB_DIR}/xawt)
      link_directories(${JAVA_LIB_DIR}/xawt)
    endif()
    if(EXISTS ${JAVA_LIB_DIR}/client)
      link_directories(${JAVA_LIB_DIR}/client)
    endif()
  endif()
  set(CMAKE_SKIP_RPATH 0)

  #tests
  add_subdirectory(tests)

  #pro
  add_subdirectory(pro)
endif()
