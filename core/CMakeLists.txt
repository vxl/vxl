
LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vcl
${allvxl_BINARY_DIR}/vxl/vpl
${allvxl_BINARY_DIR}/vxl/vil
)

LINK_LIBRARIES(
vcl
vpl
vil
)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

SUBDIRS(vpl vnl vil)

WIN32_DEFINES(-DBUILD_DLL)