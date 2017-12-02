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

set(HDFS_FOUND "NO")

find_package(JNI)

if(UNIX)

  if(JAVA_INCLUDE_PATH)

    find_path( HDFS_INCLUDE_PATH NAMES hdfs.h PATHS /usr/local/hadoop/src/c++/libhdfs )
    find_path( HDFS_LIBRARY_PATH libhdfs.so PATHS /usr/local/hadoop/c++/Linux-amd64-64/lib )
    find_library( HDFS_LIBRARY NAMES libhdfs.so PATHS /usr/local/hadoop/c++/Linux-amd64-64/lib )
    if(HDFS_INCLUDE_PATH)
      if(HDFS_LIBRARY_PATH)
        if(HDFS_LIBRARY)
          set( HDFS_FOUND "YES")
          set( HDFS_INCLUDE_DIRS ${HDFS_INCLUDE_PATH} ${JNI_INCLUDE_DIRS} )
          set( HDFS_LIBRARIES ${HDFS_LIBRARY} ${JNI_LIBRARIES} )
          message( STATUS "Found HDFS" ${HDFS_INCLUDE_PATH} )

        endif()
      endif()
    endif()

  endif()
endif()
