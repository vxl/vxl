# ./brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications
INCLUDE(${BRL_INCLUDE_DIR}/bmods/find_xerces.cmake)

# segmentation feature detection library
SUBDIRS(sdet)

IF(HAS_XERCES)
# segmentation view library
SUBDIRS(segv)

# segmentation test executables
SUBDIRS(sbin)
ENDIF(HAS_XERCES)
