#
#  gel/vgel/examples/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

LINK_LIBRARIES( vidl vgel vgel_kl mvl)

INCLUDE(${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt)

ADD_EXECUTABLE( kl_tracker kl_tracker_sources )

SOURCE_FILES( kl_tracker_sources
kl_tracker 
)
