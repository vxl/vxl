# mul/vil2/CMakeLists.txt

SET( vil2_sources 

  # basic things
  vil2_memory_chunk.cxx vil2_memory_chunk.h
  vil2_image_view_base.cxx vil2_image_view_base.h
  vil2_image_view.cxx vil2_image_view.h vil2_image_view.txx
  vil2_image_data.cxx vil2_image_data.h

  vil2_file_format.cxx vil2_file_format.h
  vil2_load.cxx vil2_load.h

  # file format readers/writers (see below for conditional ones)
  file_formats/vil2_pnm.cxx file_formats/vil2_pnm.h

  # Operations
  vil2_image_view_functions.txx vil2_image_view_functions.h

  # Stuff
  vil2_smart_ptr.txx vil2_smart_ptr.h
)


AUX_SOURCE_DIRECTORY(Templates vil2_sources)

ADD_LIBRARY(vil2 ${vil2_sources})

TARGET_LINK_LIBRARIES( vil2 vil vcl )


#SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
