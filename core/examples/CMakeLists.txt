add_executable(  example_create_image_vil
   create_image_vil.cxx
)
target_link_libraries( example_create_image_vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil )

add_subdirectory(vnl_calc)
