# ./gel/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

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
