# ./gel/CMakeLists.txt

PROJECT( gel )

SET( GEL_INCLUDE_DIR ${gel_SOURCE_DIR} CACHE STATIC "GE package include files" )

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

# spatial objects
SUBDIRS(vsol)

# topology
SUBDIRS(vtol gst)

# digital  geometry
SUBDIRS(vdgl)

# van-duc region finder (ported from TargetJr)
SUBDIRS(gevd)

# matcher
SUBDIRS(geml)

# Multi-view database
SUBDIRS(gmvl)

# Dense matcher
SUBDIRS(vsrl vmal)

# Triangulation code
SUBDIRS(gtrl)

# octree stuff from Geoff's thesis
SUBDIRS(octree)

# photogrammetry in mrc/vpgl,
# coordinate systems add-on in mrc/vcsl
SUBDIRS(mrc)

# Kanade-Lucas-Tomasi feature tracker
SUBDIRS(vgel gkll)

# Experiments with the vidl library and vgui (JLM)
SUBDIRS(jvid)
