# This is prip/vpyr/examples/simple_segmentation/CMakeLists.txt

LINK_LIBRARIES(vpyr vil)

ADD_EXECUTABLE(vpyr_example_simple_segmentation
               simple_segmentation.cxx
               simple_segmentation_builder.cxx
               simple_segmentation_builder.h
              )
