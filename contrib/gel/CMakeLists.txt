# ./gel/CMakeLists.txt

PROJECT( gel )

SET( GEL_INCLUDE_DIR ${gel_SOURCE_DIR} CACHE STATIC "GE package include files" )

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

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

# Kanade-Lucas-Tomasi feature tracker
SUBDIRS(vgel/kl vgel gkll)

# Dense matcher
SUBDIRS(vsrl vmal)

# Triangulation code
SUBDIRS(gtrl)

# octree stuff from Geoff's thesis
SUBDIRS(octree)

# photogrammetry in mrc/vpgl,
# coordinate systems add-on in mrc/vcsl
SUBDIRS(mrc)

# Experiments with the vidl library and vgui (JLM)
SUBDIRS(jvid)

# Intensity face attributes (MPP)
SUBDIRS(vifa)
