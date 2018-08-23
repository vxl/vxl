# core/examples/merge_libs

# Create a single library containing targets from (most) core vxl libraries
add_library(all_vxl_libs $<TARGET_OBJECTS:vcl-obj>
                     $<TARGET_OBJECTS:vsl-obj>
                     $<TARGET_OBJECTS:vbl-obj>
                     $<TARGET_OBJECTS:vbl_io-obj>
                     $<TARGET_OBJECTS:vgl-obj>
                     $<TARGET_OBJECTS:vgl_io-obj>
                     $<TARGET_OBJECTS:vgl_algo-obj>
                     $<TARGET_OBJECTS:v3p_netlib-obj>
                     $<TARGET_OBJECTS:vnl-obj>
                     $<TARGET_OBJECTS:vnl_algo-obj>
                     $<TARGET_OBJECTS:vnl_io-obj>
                     $<TARGET_OBJECTS:vul-obj>
                     $<TARGET_OBJECTS:vil-obj>
                     $<TARGET_OBJECTS:vil_io-obj>
                     $<TARGET_OBJECTS:vil_algo-obj>
                     )

# Check that program that uses a range of vxl classes links using just all_vxl_libs:
if (NOT VXL_VIL_INCLUDE_IMAGE_IO)
  # This assumes that no image file format readers (e.g. png, jpg etc) are required,
  # as they are not currently listed within all_vxl_libs
  add_executable(example_test_link_all_vxl test_link_all_vxl.cxx)
  target_link_libraries(example_test_link_all_vxl all_vxl_libs)
endif ()
