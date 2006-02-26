# mul/tools/CMakeLists.txt
#
#   Ian Scott
#

ADD_EXECUTABLE( concat_3d_images concat_3d_images.cxx )
TARGET_LINK_LIBRARIES(concat_3d_images vimt3d vil3d vul vbl)
