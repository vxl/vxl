#
#  gel/vgel/examples/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

LINK_LIBRARIES(
vidl
vtol
vsol
vgel
vgel_kl
mvl
)

INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt)

LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vxl/vidl

${allvxl_BINARY_DIR}/gel/vtol
${allvxl_BINARY_DIR}/gel/vgel
${allvxl_BINARY_DIR}/gel/vgel/kl
${allvxl_BINARY_DIR}/gel/vsol

${allvxl_BINARY_DIR}/oxl/mvl
)

ADD_EXECUTABLE( kl_tracker kl_tracker_sources )

SOURCE_FILES( kl_tracker_sources
kl_tracker 
)
