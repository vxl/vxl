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

ADD_EXECUTABLE( print_image print_image.cxx )
TARGET_LINK_LIBRARIES(print_image vimt3d mbl)

ADD_EXECUTABLE( image3d_math image3d_math.cxx )
TARGET_LINK_LIBRARIES(image3d_math vimt3d mbl)

ADD_EXECUTABLE( index_sort index_sort.cxx )
TARGET_LINK_LIBRARIES(index_sort mbl)

ADD_EXECUTABLE( multiplane_image multiplane_image.cxx )
TARGET_LINK_LIBRARIES(multiplane_image vimt3d mbl)

ADD_EXECUTABLE( tile_images_2d tile_images_2d.cxx )
TARGET_LINK_LIBRARIES(tile_images_2d vil_algo vimt mbl)
