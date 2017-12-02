# conversions/geometry/CMakeLists.txt

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

set(geometry_sources
 osl_to_vdgl.h
 DigitalCurve_to_vdgl.h
)

INSTALL_NOBASE_HEADER_FILES(include/vxl/contrib/conversions/geometry ${geometry_sources})
