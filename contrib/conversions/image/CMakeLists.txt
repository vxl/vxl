# conversions/image/CMakeLists.txt

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

SET(image_sources
 Image_to_vil.h
 vbl_array_to_vil.h
 vil_to_Image.h
 vil_to_vbl_array.h
)
