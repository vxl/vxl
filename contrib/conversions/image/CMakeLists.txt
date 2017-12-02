# conversions/image/CMakeLists.txt

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

set(image_sources
 Image_to_vil1.h
 vbl_array_to_vil1.h
 vil1_to_Image.h
 vil1_to_vbl_array.h
)

INSTALL_NOBASE_HEADER_FILES(include/vxl/contrib/conversions/image ${image_sources})
