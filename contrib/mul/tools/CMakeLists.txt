# mul/tools/CMakeLists.txt
#
#   Ian Scott
#

ADD_EXECUTABLE( concat_3d_images concat_3d_images.cxx )
TARGET_LINK_LIBRARIES(concat_3d_images vimt3d vil3d vul vbl)

ADD_EXECUTABLE( print_mbl_log_properties print_mbl_log_properties.cxx )
TARGET_LINK_LIBRARIES(print_mbl_log_properties mbl)

ADD_EXECUTABLE( crop_image_3d crop_image_3d.cxx )
TARGET_LINK_LIBRARIES(crop_image_3d vimt3d mbl vgl)

ADD_EXECUTABLE( print_image print_image.cxx )
TARGET_LINK_LIBRARIES(print_image vimt3d mbl vgl vil)

ADD_EXECUTABLE( image3d_math image3d_math.cxx )
TARGET_LINK_LIBRARIES(image3d_math vimt3d mbl vgl vnl)

ADD_EXECUTABLE( image3d_split image3d_split.cxx )
TARGET_LINK_LIBRARIES(image3d_split vimt3d mbl vnl)

ADD_EXECUTABLE( index_sort index_sort.cxx )
TARGET_LINK_LIBRARIES(index_sort mbl)

ADD_EXECUTABLE( multiplane_image multiplane_image.cxx )
TARGET_LINK_LIBRARIES(multiplane_image vimt3d mbl vil)

ADD_EXECUTABLE( tile_images_2d tile_images_2d.cxx )
TARGET_LINK_LIBRARIES(tile_images_2d vil_algo vimt mbl vil)

ADD_EXECUTABLE( reflect_image_2d reflect_image_2d.cxx )
TARGET_LINK_LIBRARIES(reflect_image_2d vimt mbl)

ADD_EXECUTABLE( reflect_image_3d reflect_image_3d.cxx )
TARGET_LINK_LIBRARIES(reflect_image_3d vimt3d mbl)
