#
# ./gel/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)

SUBDIRS(gbl gmvl gst vsol vtol vdgl vsrl vace)

#SUBDIRS(cmaketests)
