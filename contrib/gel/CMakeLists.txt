#
# ./gel/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

SUBDIRS(gbl gmvl gst vsol vtol vdgl vsrl)

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)

IF(HAS_OPENGL)
 SUBDIRS(vace)
ENDIF(HAS_OPENGL)


# S#UBDIRS(cmaketests)
