#
# try to find libhdfs (C API to interact with Hadoop File System )
#
# HDFS_INCLUDE_DIR, where to find hdfs.h, etc.
# HDFS_LIBRARIES, the libraries to link against to use libhdfs.
# HDFS_FOUND, If false, do not try to use libhdfs.
#
#
# A note to run executables that will use libdhfs, it requires the environment variable CLASSPATH to be set
# for that one must get the classpath from hadoop:
# TMP=$(/usr/local/hadoop/bin/hadoop classpath)
# and then:
# CLASSPATH=$CLASSPATH:$TMP
#
#

SET(HDFS_FOUND "NO")

INCLUDE (${CMAKE_ROOT}/Modules/FindJNI.cmake)

IF (UNIX)

  IF (JAVA_INCLUDE_PATH) 

    FIND_PATH( HDFS_INCLUDE_PATH NAMES hdfs.h PATHS /usr/local/hadoop/src/c++/libhdfs )
    FIND_PATH( HDFS_LIBRARY_PATH libhdfs.so PATHS /usr/local/hadoop/c++/Linux-amd64-64/lib )
    FIND_LIBRARY( HDFS_LIBRARY NAMES libhdfs.so PATHS /usr/local/hadoop/c++/Linux-amd64-64/lib )
    IF (HDFS_INCLUDE_PATH)
      IF (HDFS_LIBRARY_PATH)
        IF (HDFS_LIBRARY)
          SET( HDFS_FOUND "YES")
          SET( HDFS_INCLUDE_DIRS ${HDFS_INCLUDE_PATH} ${JNI_INCLUDE_DIRS} )
          SET( HDFS_LIBRARIES ${HDFS_LIBRARY} ${JNI_LIBRARIES} )
          MESSAGE( STATUS "Found HDFS" ${HDFS_INCLUDE_PATH} )

        ENDIF (HDFS_LIBRARY)
      ENDIF (HDFS_LIBRARY_PATH)
    ENDIF (HDFS_INCLUDE_PATH)

  ENDIF (JAVA_INCLUDE_PATH)
ENDIF (UNIX)
