# ./gel/CMakeLists.txt

PROJECT( gel )

doxygen_add_package(contrib/gel "GE Libraries")

SET( GEL_INCLUDE_DIR ${gel_SOURCE_DIR} CACHE STATIC "GE package include files" )
SET( GEL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/gel
  CACHE INTERNAL "GE package include files"
)

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

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

# Kanade-Lucas-Tomasi feature tracker;
IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  add_subdirectory(vgel/kl)
  add_subdirectory(vgel)
  add_subdirectory(gkll)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)

# Dense matcher
add_subdirectory(vmal)

# Triangulation code
add_subdirectory(gtrl)

# octree stuff from Geoff's thesis
IF(BUILD_OXL)
  add_subdirectory(octree)
ENDIF(BUILD_OXL)

# photogrammetry in mrc/vpgl,
# coordinate systems add-on in mrc/vcsl
add_subdirectory(mrc)

# Intensity face attributes (MPP)
add_subdirectory(vifa)
