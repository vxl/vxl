
#
#  gel/vgel/examples/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

LINK_LIBRARIES(
vcl
jpeg
vbl
vnl
vil
vil_file_formats
vgl
vidl
vtol
vsol
vgel
vgel_kl
mvl
)

LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vcl

${allvxl_BINARY_DIR}/v3p/jpeg

${allvxl_BINARY_DIR}/vxl/vbl
${allvxl_BINARY_DIR}/vxl/vnl
${allvxl_BINARY_DIR}/vxl/vil
${allvxl_BINARY_DIR}/vxl/vil/file_formats
${allvxl_BINARY_DIR}/vxl/vgl
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
