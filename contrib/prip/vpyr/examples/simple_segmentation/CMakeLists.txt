# This is prip/vpyr/examples/simple_segmentation/CMakeLists.txt

link_libraries(vpyr vmap ${VXL_LIB_PREFIX}vil)

add_executable(vpyr_example_simple_segmentation
               simple_segmentation.cxx
               simple_segmentation_builder.cxx
               simple_segmentation_builder.h
              )
