ADD_EXECUTABLE(  example_create_image_vil
   create_image_vil.cxx
)
TARGET_LINK_LIBRARIES( example_create_image_vil vul vil )

ADD_EXECUTABLE(  example_create_image_vil2
   create_image_vil2.cxx
)
TARGET_LINK_LIBRARIES( example_create_image_vil2 vul vil2 )

SUBDIRS( vnl_calc )
