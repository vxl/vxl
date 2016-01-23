# core/vidl/examples/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( HAS_VGUI )

include_directories( ${VXLCORE_INCLUDE_DIR} )

set(vidl_player_sources
    vidl_player_main.cxx
    vidl_player_menus.cxx      vidl_player_menus.h
    vidl_player_manager.cxx    vidl_player_manager.h
   )

add_executable(vidl_player MACOSX_BUNDLE ${vidl_player_sources})
target_link_libraries(vidl_player vidl vidl_gui vul)

endif( HAS_VGUI )


if( DIRECTSHOW_FOUND )

# An example use of vidl_dshow_file_istream
add_executable(vidl_dshow_file_example vidl_dshow_file_example.cxx)
target_link_libraries(vidl_dshow_file_example vidl)
if(BUILD_MUL)
# An example use of vidl_dshow_live_istream
include_directories( ${VXL_MUL_INCLUDE_DIR} )
add_executable(vidl_dshow_live_example vidl_dshow_live_example.cxx)
target_link_libraries(vidl_dshow_live_example vidl mbl)
endif(BUILD_MUL)
endif( DIRECTSHOW_FOUND )

if( VIDEODEV_FOUND )
   add_executable(vidl_v4l_example  vidl_v4l_live_example.cxx)
   target_link_libraries(vidl_v4l_example vidl)
endif( VIDEODEV_FOUND )


if( VIDEODEV2_FOUND )
   add_executable(vidl_v4l2_example  vidl_v4l2_live_example.cxx)
   target_link_libraries(vidl_v4l2_example vidl)
endif( VIDEODEV2_FOUND )


add_executable( vidl_transcode_video
  vidl_transcode_video.cxx
)
target_link_libraries( vidl_transcode_video vidl vul vil )
