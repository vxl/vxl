# mul/vil3d/CMakeLists.txt

SET( vil3d_sources ${vil3d_sources}

  # basic things
  vil3d_image_view_base.cxx vil3d_image_view_base.h
  vil3d_image_view.cxx      vil3d_image_view.h      vil3d_image_view.txx
  vil3d_print.h
  vil3d_plane.h
  vil3d_slice.h
  vil3d_crop.h
  vil3d_trilin_interp.h
  vil3d_sample_profile_trilin.h  vil3d_sample_profile_trilin.txx
)

AUX_SOURCE_DIRECTORY(Templates vil3d_sources)

ADD_LIBRARY(vil3d ${vil3d_sources})

TARGET_LINK_LIBRARIES( vil3d vil2 vil vcl )


SUBDIRS(algo io)

#IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
