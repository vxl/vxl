#
# ./gel/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

# basics
SUBDIRS(gbl)

# van-duc region finder (ported from TargetJr)
 SUBDIRS(gevd)

# matcher
 SUBDIRS(geml)

# Multi-view database
SUBDIRS(gmvl)

# Dense matcher
SUBDIRS(vsrl)

# Triangulation code
 SUBDIRS(gtrl)

# spatial objects
SUBDIRS(vsol)

# topology
SUBDIRS(gst vtol)

# digital  geometry
SUBDIRS(vdgl)

# photogrammetry
 SUBDIRS(mrc/vpgl)

# coordinate systems addon
 SUBDIRS(mrc/vcsl)

# Kanade-Lucas-Tomasi feature tracker
 SUBDIRS(vgel gkll)

# Dense matcher
 SUBDIRS(vmal)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
