# mul/vil2/CMakeLists.txt

SET( vil2_sources

  # basic things
  vil2_memory_chunk.cxx    vil2_memory_chunk.h
  vil2_image_view_base.cxx vil2_image_view_base.h
  vil2_image_view.cxx      vil2_image_view.h      vil2_image_view.txx
  vil2_image_resource.cxx  vil2_image_resource.h

  vil2_file_format.cxx vil2_file_format.h
  vil2_memory_image.cxx vil2_memory_image.h


  # file format readers/writers (see below for conditional ones)
  file_formats/vil2_pnm.cxx file_formats/vil2_pnm.h

  # image operations
  vil2_crop.cxx  vil2_crop.h
  vil2_clamp.cxx vil2_clamp.h
  vil2_transpose.cxx  vil2_transpose.h
  vil2_flip.cxx  vil2_flip.h
  vil2_plane.cxx vil2_plane.h
  vil2_math.h
  vil2_load.cxx  vil2_load.h
  vil2_save.cxx  vil2_save.h
  vil2_copy.txx  vil2_copy.h
  vil2_new.cxx   vil2_new.h
  vil2_image_view_functions.cxx vil2_image_view_functions.h vil2_image_view_functions.txx

  # Sampling Operations
  vil2_bilin_interp.h
  vil2_sample_profile_bilin.txx vil2_sample_profile_bilin.h
  vil2_sample_grid_bilin.txx    vil2_sample_grid_bilin.h

  # Stuff
  vil2_smart_ptr.txx vil2_smart_ptr.h
  vil2_pixel_traits.h
  vil2_property.h
  vil2_pixel_format.cxx vil2_pixel_format.h
)


AUX_SOURCE_DIRECTORY(Templates vil2_sources)

ADD_LIBRARY(vil2 ${vil2_sources})

TARGET_LINK_LIBRARIES( vil2 vil vcl )


SUBDIRS(algo io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
