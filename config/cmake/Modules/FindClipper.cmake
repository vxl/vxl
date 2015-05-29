#
# Try to find Clipper
#
# mirrors FindQv.cmake; although could mirror FindTIFF.cmake/FindGEOTIFF.cmake

SET( CLIPPER_FOUND "YES" )
SET( CLIPPER_INCLUDE_DIR ${vxl_SOURCE_DIR}/v3p )
SET( CLIPPER_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/v3p )
SET( CLIPPER_LIBRARIES clipper )
