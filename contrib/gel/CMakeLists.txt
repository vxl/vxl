#
# ./gel/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

SUBDIRS(gbl gmvl gst vsol vtol vdgl vsrl)

IF(HAS_OPENGL)
 SUBDIRS(vace)
ENDIF(HAS_OPENGL)

# S#UBDIRS(cmaketests)
