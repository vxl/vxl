# vxl/CMakeLists.txt

PROJECT( vxlcore )

doxygen_add_book(core "VXL overview documentation")

doxygen_add_package(core "Core Vision Libraries")
doxygen_add_package(core-L2 "Level-2 Vision Libraries")
doxygen_add_package(core-L2E "Level-2 Vision Libraries (Experimental)")
doxygen_add_package(core-test "Test Framework")

SET(global_sources
 vxl_copyright.h
 vxl_version.h
 vxl_config.h.in
)

INSTALL_NOBASE_HEADER_FILES(/include/vxl/core ${global_sources})

IF (BUILD_CORE_UTILITIES)
  # POSIX layer
  add_subdirectory(vpl)

  # utilities
  add_subdirectory(vul)

  # "basics" (templates)
  add_subdirectory(vbl)
ENDIF (BUILD_CORE_UTILITIES)


# numerics
IF (BUILD_CORE_NUMERICS)
  add_subdirectory(vnl)
ENDIF (BUILD_CORE_NUMERICS)


# geometry (including computational geometry)
IF (BUILD_CORE_GEOMETRY)
  add_subdirectory(vgl)
ENDIF (BUILD_CORE_GEOMETRY)


IF (BUILD_CORE_IMAGING)
  # image
  add_subdirectory(vil)

  IF (BUILD_CORE_NUMERICS)
    add_subdirectory (vil/algo)
  ENDIF (BUILD_CORE_NUMERICS)
ENDIF (BUILD_CORE_IMAGING)


##- if(NOT BUILD_NONDEPRECATED_ONLY)  ## NOTE THIS CAN NOT BE DEPRECATED
#                                     # more discussion needed.
#                                     # deprecating vil1 involves MANY
#                                     # other deprecations that need further
#                                     # review
# Old image library (deprecated)
IF (BUILD_CORE_IMAGING ) ## BRL still depends on vil1 AND NOT BUILD_NONDEPRECATED_ONLY)
  add_subdirectory(vil1)
ENDIF (BUILD_CORE_IMAGING)


IF (BUILD_CORE_SERIALISATION)
  # Fast Binary IO
  add_subdirectory(vsl)

  IF (BUILD_CORE_UTILITIES)
    add_subdirectory(vul/io)
    add_subdirectory(vbl/io)
  ENDIF (BUILD_CORE_UTILITIES)

  IF (BUILD_CORE_NUMERICS)
    add_subdirectory(vnl/io)
    add_subdirectory(vnl/xio)
  ENDIF (BUILD_CORE_NUMERICS)

  IF (BUILD_CORE_GEOMETRY)
    add_subdirectory(vgl/io)
    add_subdirectory(vgl/xio)
  ENDIF (BUILD_CORE_GEOMETRY)

  IF (BUILD_CORE_IMAGING)
    add_subdirectory(vil/io)
  ENDIF (BUILD_CORE_IMAGING)
ENDIF (BUILD_CORE_SERIALISATION)


# coordinate systems
IF (BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)
  add_subdirectory(vcsl)
  add_subdirectory(vpgl)
ENDIF (BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)


# probability distributions
IF (BUILD_CORE_PROBABILITY)
  add_subdirectory(vpdl)
ENDIF (BUILD_CORE_PROBABILITY)


# multi-platform GUI toolkit
# Default the dashboard builds to "YES" so that
# we have some clients that try to compile vgui
CMAKE_DEPENDENT_OPTION( BUILD_VGUI "Build VGUI" OFF
                         "BUILD_CORE_GEOMETRY;BUILD_CORE_NUMERICS;BUILD_CORE_UTILITIES;BUILD_CORE_SERIALISATION;BUILD_CORE_IMAGING" OFF )
if(BUILD_VGUI)
  add_subdirectory(vgui)
endif()


# video
CMAKE_DEPENDENT_OPTION (BUILD_CORE_VIDEO "Build VXL's core video libraries" ON
                        "BUILD_CORE_UTILITIES;BUILD_CORE_IMAGING" OFF)
if (BUILD_CORE_VIDEO)
  add_subdirectory(vidl)
  SET (CORE_VIDEO_FOUND ON CACHE INTERNAL "VXL core video libraries built")
else()
  SET (CORE_VIDEO_FOUND OFF CACHE INTERNAL "VXL core video libraries built")
endif ()

# common test executable
add_subdirectory(testlib)

# Tests that check and output the vxl configuration
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
if( BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

