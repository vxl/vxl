# contrib/brl/bbas/bil/CMakeLists.txt
# Brown Image Library


SET(bil_sources 
    bil_math.h
    bil_bounded_image_view.h    bil_bounded_image_view.txx
   )

AUX_SOURCE_DIRECTORY(Templates bil_sources)

ADD_LIBRARY(bil  ${bil_sources})

TARGET_LINK_LIBRARIES(bil vil vil_io vsl)

SUBDIRS( algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
