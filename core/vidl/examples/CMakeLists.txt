# vxl/vidl/examples/CMakeLists.txt

ADD_EXECUTABLE( vidl_mpegcodec_example vidl_mpegcodec_example.cxx )
TARGET_LINK_LIBRARIES( vidl_mpegcodec_example vidl vbl )

ADD_EXECUTABLE( vidl_video2images vidl_video2images.cxx )
TARGET_LINK_LIBRARIES( vidl_video2images vidl vil vul vcl )


SET(vidl_player_sources 
    vidl_player_main.cxx 
    vidl_player_menus.cxx      vidl_player_menus.h
	vidl_player_manager.cxx    vidl_player_manager.h
)

ADD_EXECUTABLE (vidl_player ${vidl_player_sources})
TARGET_LINK_LIBRARIES(vidl_player vidl vgui)