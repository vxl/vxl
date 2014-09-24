# mul/vil3d/tools/CMakeLists.txt
#
#  Tim Cootes
#
LINK_LIBRARIES(vil3d vnl vil vul vil3d_algo)

ADD_EXECUTABLE(vil3d_slice_image vil3d_slice_image.cxx)
ADD_EXECUTABLE(vil3d_byte_image_histo vil3d_byte_image_histo.cxx)
