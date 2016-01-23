# vxl/CMakeLists.txt

project( vxlcore )

doxygen_add_book(core "VXL overview documentation")

doxygen_add_package(core "Core Vision Libraries")
doxygen_add_package(core-L2 "Level-2 Vision Libraries")
doxygen_add_package(core-L2E "Level-2 Vision Libraries (Experimental)")
doxygen_add_package(core-test "Test Framework")

set(global_sources
 vxl_copyright.h
 vxl_version.h
 vxl_config.h.in
)

INSTALL_NOBASE_HEADER_FILES(/include/vxl/core ${global_sources})

if(BUILD_CORE_UTILITIES)
  # POSIX layer
  add_subdirectory(vpl)

  # utilities
  add_subdirectory(vul)

  # "basics" (templates)
  add_subdirectory(vbl)
endif(BUILD_CORE_UTILITIES)


# numerics
if(BUILD_CORE_NUMERICS)
  add_subdirectory(vnl)
endif(BUILD_CORE_NUMERICS)


# geometry (including computational geometry)
if(BUILD_CORE_GEOMETRY)
  add_subdirectory(vgl)
endif(BUILD_CORE_GEOMETRY)


if(BUILD_CORE_IMAGING)
  # image
  add_subdirectory(vil)

  if(BUILD_CORE_NUMERICS)
    add_subdirectory (vil/algo)
  endif(BUILD_CORE_NUMERICS)
endif(BUILD_CORE_IMAGING)


##- if(NOT BUILD_NONDEPRECATED_ONLY)  ## NOTE THIS CAN NOT BE DEPRECATED
#                                     # more discussion needed.
#                                     # deprecating vil1 involves MANY
#                                     # other deprecations that need further
#                                     # review
# Old image library (deprecated)
if(BUILD_CORE_IMAGING ) ## BRL still depends on vil1 AND NOT BUILD_NONDEPRECATED_ONLY)
  add_subdirectory(vil1)
endif(BUILD_CORE_IMAGING)


if(BUILD_CORE_SERIALISATION)
  # Fast Binary IO
  add_subdirectory(vsl)

  if(BUILD_CORE_UTILITIES)
    add_subdirectory(vul/io)
    add_subdirectory(vbl/io)
  endif(BUILD_CORE_UTILITIES)

  if(BUILD_CORE_NUMERICS)
    add_subdirectory(vnl/io)
    add_subdirectory(vnl/xio)
  endif(BUILD_CORE_NUMERICS)

  if(BUILD_CORE_GEOMETRY)
    add_subdirectory(vgl/io)
    add_subdirectory(vgl/xio)
  endif(BUILD_CORE_GEOMETRY)

  if(BUILD_CORE_IMAGING)
    add_subdirectory(vil/io)
  endif(BUILD_CORE_IMAGING)
endif(BUILD_CORE_SERIALISATION)


# coordinate systems
if(BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)
  add_subdirectory(vcsl)
  add_subdirectory(vpgl)
endif(BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)


# probability distributions
if(BUILD_CORE_PROBABILITY)
  add_subdirectory(vpdl)
endif(BUILD_CORE_PROBABILITY)


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
  set(CORE_VIDEO_FOUND ON CACHE INTERNAL "VXL core video libraries built")
else()
  set(CORE_VIDEO_FOUND OFF CACHE INTERNAL "VXL core video libraries built")
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

