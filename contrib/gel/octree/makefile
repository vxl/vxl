USES += vxl oxl gel
include ${IUEROOT}/vxl/vil/config.mk
USE_NETLIB := 1

PROGRAM = octree

SOURCES += main.cxx
SOURCES += VoxmapPoints.cxx
SOURCES += VoxmapImagePoints.cxx
SOURCES += Voxel.cxx
SOURCES += OctreeLevel.cxx
SOURCES += Cube.cxx
SOURCES += ConvexHull.cxx
SOURCES += BaseCube.cxx

IULIBS += -lmvl -lvgl -lvbl -lvul -lvnl-algo -lvnl -lvil

################################
include ${IUEROOT}/config/top.mk
################################
