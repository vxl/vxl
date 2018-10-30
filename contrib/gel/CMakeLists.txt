# ./gel/CMakeLists.txt

project( gel )

doxygen_add_package(contrib/gel "GE Libraries")

set( GEL_INCLUDE_DIR ${gel_SOURCE_DIR} CACHE STATIC "GE package include files" )
set( GEL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/gel
  CACHE INTERNAL "GE package include files"
)

include_directories( ${GEL_INCLUDE_DIR} )

# spatial objects
add_subdirectory(vsol)

# digital  geometry
add_subdirectory(vdgl)

# topology
add_subdirectory(vtol)
add_subdirectory(gst)

# van-duc region finder (ported from TargetJr)
add_subdirectory(gevd)

# matcher
add_subdirectory(geml)

# Multi-view database
add_subdirectory(gmvl)

# Dense matcher
add_subdirectory(vmal)

# Triangulation code
add_subdirectory(gtrl)

# octree stuff from Geoff's thesis
if(VXL_BUILD_OXL)
  add_subdirectory(octree)
endif()

# photogrammetry in mrc/vpgl,
# coordinate systems add-on in mrc/vcsl
add_subdirectory(mrc)

# Intensity face attributes (MPP)
add_subdirectory(vifa)
