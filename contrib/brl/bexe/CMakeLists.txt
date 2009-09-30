# brl/bexe/CMakeLists.txt
# main package for basic operations common to all libraries

# video editing executables
IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  SUBDIRS(vded)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
