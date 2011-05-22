# core/vidl/examples/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

IF( HAS_VGUI )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET(vidl_player_sources
    vidl_player_main.cxx
    vidl_player_menus.cxx      vidl_player_menus.h
    vidl_player_manager.cxx    vidl_player_manager.h
   )

ADD_EXECUTABLE (vidl_player MACOSX_BUNDLE ${vidl_player_sources})
TARGET_LINK_LIBRARIES(vidl_player vidl vidl_gui vul)

ENDIF( HAS_VGUI )


IF( DIRECTSHOW_FOUND )

# An example use of vidl_dshow_file_istream
ADD_EXECUTABLE (vidl_dshow_file_example vidl_dshow_file_example.cxx)
TARGET_LINK_LIBRARIES(vidl_dshow_file_example vidl)
IF(BUILD_MUL)
# An example use of vidl_dshow_live_istream
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
ADD_EXECUTABLE (vidl_dshow_live_example vidl_dshow_live_example.cxx)
TARGET_LINK_LIBRARIES(vidl_dshow_live_example vidl mbl)
ENDIF(BUILD_MUL)
ENDIF( DIRECTSHOW_FOUND )

IF ( VIDEODEV_FOUND )
   ADD_EXECUTABLE (vidl_v4l_example  vidl_v4l_live_example.cxx)
   TARGET_LINK_LIBRARIES(vidl_v4l_example vidl)
ENDIF ( VIDEODEV_FOUND )


IF ( VIDEODEV2_FOUND )
   ADD_EXECUTABLE (vidl_v4l2_example  vidl_v4l2_live_example.cxx)
   TARGET_LINK_LIBRARIES(vidl_v4l2_example vidl)
ENDIF ( VIDEODEV2_FOUND )


ADD_EXECUTABLE( vidl_transcode_video
  vidl_transcode_video.cxx
)
TARGET_LINK_LIBRARIES( vidl_transcode_video vidl vul vil )
