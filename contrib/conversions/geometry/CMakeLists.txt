# conversions/geometry/CMakeLists.txt

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

SET(geometry_sources
 osl_to_vdgl.h
 DigitalCurve_to_vdgl.h
)

INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/conversions/geometry ${geometry_sources})
