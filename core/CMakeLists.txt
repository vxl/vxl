
LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vcl
${allvxl_BINARY_DIR}/vxl/vpl
${allvxl_BINARY_DIR}/vxl/vnl
${allvxl_BINARY_DIR}/vxl/vnl/algo
${allvxl_BINARY_DIR}/vxl/vil
${allvxl_BINARY_DIR}/vxl/vil/file_formats
${allvxl_BINARY_DIR}/vbl
)

LINK_LIBRARIES(
vcl
vpl
vil
vil_file_formats
vnl
vnl_algo
vgl
)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

SUBDIRS(vbl vpl vil vnl vgl)

