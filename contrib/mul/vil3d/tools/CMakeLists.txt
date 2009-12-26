# mul/vil3d/tools/CMakeLists.txt
#
#  Tim Cootes
#
LINK_LIBRARIES(vil3d vnl vil vul)

ADD_EXECUTABLE(vil3d_slice_image vil3d_slice_image.cxx)
