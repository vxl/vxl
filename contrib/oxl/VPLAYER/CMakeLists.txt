# oxl/VPLAYER/CMakeLists.txt

IF(WIN32)
 ADD_EXECUTABLE( VPLAYER VPLAYER_sources )
 SOURCE_FILES( VPLAYER_sources
   easy2D_loader
   easy2D_sequencer
   vplayer
   vplayer_file
   vplayer_geometry
   vplayer_video
 )

 LINK_LIBRARIES( nafxcw )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
ENDIF(WIN32)
