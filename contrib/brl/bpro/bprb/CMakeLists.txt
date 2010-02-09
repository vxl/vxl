# brl/bpro/bprb/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )


SET(bprb_sources
   bprb_parameters.cxx             bprb_parameters.h            bprb_parameters_sptr.h
   bprb_parameters.txx
   bprb_process.cxx                bprb_process.h               bprb_process_sptr.h
   bprb_process_ext.cxx            bprb_process_ext.h  
   bprb_process_manager.txx        bprb_process_manager.h
   bprb_batch_process_manager.cxx  bprb_batch_process_manager.h
   bprb_null_process.cxx           bprb_null_process.h
   bprb_func_process.h
   bprb_macros.h
)

AUX_SOURCE_DIRECTORY(Templates bprb_sources)
IF(UNIX)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      ADD_DEFINITIONS(-fPIC)
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  ENDIF(CMAKE_UNAME)
ENDIF(UNIX)
ADD_LIBRARY(bprb ${bprb_sources})

TARGET_LINK_LIBRARIES(bprb brdb bxml vbl vsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

