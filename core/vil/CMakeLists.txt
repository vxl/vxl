# mul/vil2/CMakeLists.txt





SET( vil2_sources 

  # basic things
  vil2_image_data.cxx vil2_image_data.h
  vil2_image_view.cxx vil2_image_view.h
  vil2_image_view_base.h


  # file format readers/writers (see below for conditional ones)
  file_formats/vil2_pnm.cxx file_formats/vil2_pnm.h

  # Operations

  # Stuff
)


AUX_SOURCE_DIRECTORY(Templates vil2_sources)

ADD_LIBRARY(vil2 ${vil2_sources})

TARGET_LINK_LIBRARIES( vil2 vil vcl )


#SUBDIRS(io)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
