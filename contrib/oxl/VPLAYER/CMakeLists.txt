# ./oxl/VPLAYER/CMakeLists.txt

IF(WIN32)
 ADD_EXECUTABLE( VPLAYER VPLAYER_sources )
 SOURCE_FILES( VPLAYER_sources
  easy2D_loader.cxx
  easy2D_sequencer.cxx
  vplayer.cxx
  vplayer_file.cxx
  vplayer_geometry.cxx
  vplayer_video.cxx
 )

 LINK_LIBRARIES( vfw32 )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/oxp/CMakeListsLink.txt )

 IF (NOT CYGWIN)
  INCLUDE(${allvxl_SOURCE_DIR}/oxl/VPLAYER/CMakeListsHeaders.txt)
 ENDIF (NOT CYGWIN)
ENDIF(WIN32)
