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
SUBDIRS(vsol)

# digital  geometry
SUBDIRS(vdgl)

# topology
SUBDIRS(vtol gst)

# van-duc region finder (ported from TargetJr)
SUBDIRS(gevd)

# matcher
SUBDIRS(geml)

# Multi-view database
SUBDIRS(gmvl)

# Kanade-Lucas-Tomasi feature tracker;
IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  SUBDIRS(vgel/kl vgel gkll)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)

# Dense matcher
SUBDIRS(vmal)

# Triangulation code
SUBDIRS(gtrl)

# octree stuff from Geoff's thesis
IF(BUILD_OXL)
  SUBDIRS(octree)
ENDIF(BUILD_OXL)

# photogrammetry in mrc/vpgl,
# coordinate systems add-on in mrc/vcsl
SUBDIRS(mrc)

# Intensity face attributes (MPP)
SUBDIRS(vifa)
