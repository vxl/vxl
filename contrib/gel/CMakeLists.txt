# ./gel/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

# matcher
SUBDIRS(geml)

# octree stuff from Geoff's thesis
SUBDIRS(octree)

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

# photogrammetry and
# coordinate systems addon
SUBDIRS(mrc)

# Kanade-Lucas-Tomasi feature tracker
SUBDIRS(vgel gkll)

# Dense matcher
SUBDIRS(vmal)

# van-duc region finder (ported from TargetJr)
SUBDIRS(gevd)

# This should *not* be included here since it will cause all programs,
# also the non-GUI ones, to link against libGL !
# Place it only in those directories that really need it.
#INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
