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

 LINK_LIBRARIES( nafxcw allvxl alloxl opengl32 glu32 )
ENDIF(WIN32)
