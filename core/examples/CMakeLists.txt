add_executable(  example_create_image_vil
   create_image_vil.cxx
)
target_link_libraries( example_create_image_vil vul vil )

add_subdirectory(vnl_calc)
