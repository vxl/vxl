# contrib/brl/bbas/bil/CMakeLists.txt
# Brown Image Library


SET(bil_sources 
    bil_math.h
    bil_bounded_image_view.h    bil_bounded_image_view.txx
    bil_raw_image_istream.h     bil_raw_image_istream.cxx
	bil_arf_image_istream.h     bil_arf_image_istream.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bil_sources)

ADD_LIBRARY(bil  ${bil_sources})

TARGET_LINK_LIBRARIES(bil vil vil_io vsl)

SUBDIRS( algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( exe )

INSTALL_TARGETS(/lib bil)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bil ${bil_sources})