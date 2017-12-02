# mul/tools/CMakeLists.txt
#
#   Ian Scott
#

add_executable( concat_3d_images concat_3d_images.cxx )
target_link_libraries(concat_3d_images vimt3d vil3d ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)

add_executable( crop_image_2d crop_image_2d.cxx )
target_link_libraries(crop_image_2d vimt mbl ${VXL_LIB_PREFIX}vgl)

add_executable( crop_image_3d crop_image_3d.cxx )
target_link_libraries(crop_image_3d vimt3d mbl ${VXL_LIB_PREFIX}vgl)

add_executable( print_image print_image.cxx )
target_link_libraries(print_image vimt3d mbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil)

add_executable( image3d_math image3d_math.cxx )
target_link_libraries(image3d_math vimt3d mbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl)

add_executable( image3d_split image3d_split.cxx )
target_link_libraries(image3d_split vimt3d mbl ${VXL_LIB_PREFIX}vnl)

add_executable( index_sort index_sort.cxx )
target_link_libraries(index_sort mbl)

add_executable( multiplane_image multiplane_image.cxx )
target_link_libraries(multiplane_image vimt3d mbl ${VXL_LIB_PREFIX}vil)

add_executable( tile_images_2d tile_images_2d.cxx )
target_link_libraries(tile_images_2d ${VXL_LIB_PREFIX}vil_algo vimt mbl ${VXL_LIB_PREFIX}vil)

add_executable( reflect_image_2d reflect_image_2d.cxx )
target_link_libraries(reflect_image_2d vimt mbl)

add_executable( reflect_image_3d reflect_image_3d.cxx )
target_link_libraries(reflect_image_3d vimt3d mbl)

add_executable( select_lines select_lines.cxx )
target_link_libraries(select_lines mbl)

add_executable( convert_rotation_3d convert_rotation_3d.cxx )
target_link_libraries(convert_rotation_3d mbl ${VXL_LIB_PREFIX}vgl_algo)

add_executable( rewrite_vsl_strings rewrite_vsl_strings.cxx )
target_link_libraries(rewrite_vsl_strings mbl)
