if (VXL_VIL_INCLUDE_IMAGE_IO)
  add_executable( example_create_image_vil create_image_vil.cxx )
  target_link_libraries( example_create_image_vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil )
endif ()

add_subdirectory(vnl_calc)

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_subdirectory(merge_libs)
endif ()
