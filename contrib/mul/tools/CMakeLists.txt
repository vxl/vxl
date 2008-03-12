# mul/tools/CMakeLists.txt
#
#   Ian Scott
#

ADD_EXECUTABLE( concat_3d_images concat_3d_images.cxx )
TARGET_LINK_LIBRARIES(concat_3d_images vimt3d vil3d vul vbl)

ADD_EXECUTABLE( print_mbl_log_properties print_mbl_log_properties.cxx )
TARGET_LINK_LIBRARIES(print_mbl_log_properties mbl)

ADD_EXECUTABLE( crop_image_3d crop_image_3d.cxx )
TARGET_LINK_LIBRARIES(crop_image_3d mbl vimt3d)
