ADD_EXECUTABLE(  example_create_image_vil
   create_image_vil.cxx
)
TARGET_LINK_LIBRARIES( example_create_image_vil vul vil )

SUBDIRS( vnl_calc )
