# ./brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications


# segmentation feature detection library
SUBDIRS(sdet)

IF(HAS_XERCES)
# segmentation view library
SUBDIRS(segv)

# segmentation test executables
SUBDIRS(sbin)
ENDIF(HAD_XERCES)
