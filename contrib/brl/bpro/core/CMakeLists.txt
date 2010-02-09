# brl/bpro/core/CMakeLists.txt
# main package for processing operations using core classes

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
# vpgl process classes 
SUBDIRS(vpgl_pro)

# vil process classes
SUBDIRS(vil_pro)

# vidl process classes (note it is expected that vidl will go to core)
SUBDIRS(vidl_pro)

# brip process classes 
SUBDIRS(brip_pro)

# vgl and bvgl process classes 
SUBDIRS(bvgl_pro)

# sdet process classes
SUBDIRS(sdet_pro)
