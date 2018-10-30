project( tbl )

doxygen_add_book(contrib/tbl "The Image Processing Library")

doxygen_add_package(contrib/tbl "Image Processing Libraries")

set( TBL_INCLUDE_DIR ${tbl_SOURCE_DIR} CACHE STATIC "TB package include files" )
set( TBL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/tbl
  CACHE INTERNAL "TB package include files"
)

include_directories(${TBL_INCLUDE_DIR})

add_subdirectory(vipl)
# Although this is the "image processing" library, it does not (necessary) depend on vil!
# vil-specific implementations are in the "vepl" specialisation library,
if(VXL_BUILD_CORE_IMAGING)
  add_subdirectory(vepl)
endif()
