PROJECT( tbl )

doxygen_add_book(contrib/tbl "The Image Processing Library")

doxygen_add_package(contrib/tbl "Image Processing Libraries")

SET( TBL_INCLUDE_DIR ${tbl_SOURCE_DIR} CACHE STATIC "TB package include files" )
SET( TBL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/tbl
  CACHE INTERNAL "TB package include files"
)

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TBL_INCLUDE_DIR})

SUBDIRS(vipl)
# Although this is the "image processing" library, it does not (necessary) depend on vil!
# vil-specific implementations are in the "vepl" specialisation library,
# and vil1-specific stuff is in vepl1:
IF (BUILD_CORE_IMAGING)
  SUBDIRS(vepl)
ENDIF (BUILD_CORE_IMAGING)
IF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)
  SUBDIRS(vepl1)
ENDIF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)
