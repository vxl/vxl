# brl/vvid/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
# find the path to the cmu camera libraries and includes
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_cmu_camera.cmake )

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)
# we have vgui enabled so ..
INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )

# sources that don't depend on the camera being present
SET(vvid_sources
    vvid_frame_trail.cxx             vvid_frame_trail.h
    vvid_file_manager.cxx            vvid_file_manager.h
    vvid_command_line_player.cxx       vvid_command_line_player.h
)
AUX_SOURCE_DIRECTORY(Templates vvid_sources)

# sources that do depend on the camera being present

IF(HAS_CMU1394CAMERA)
 ADD_DEFINITIONS(-DHAS_CMU1394CAMERA)
 SET(cmu_camera_sources
     cmu_1394_camera_params.cxx   cmu_1394_camera_params.h
     cmu_1394_camera.cxx          cmu_1394_camera.h
     vvid_live_video_tableau.cxx  vvid_live_video_tableau.h
                                  vvid_live_video_tableau_sptr.h
     vvid_live_video_manager.cxx  vvid_live_video_manager.h
     vvid_live_stereo_manager.cxx vvid_live_stereo_manager.h
 )
ENDIF(HAS_CMU1394CAMERA)

# check if we have the camera libraries and assemble accordingly
IF(HAS_CMU1394CAMERA)
 ADD_LIBRARY(vvid ${vvid_sources} ${cmu_camera_sources})
ELSE(HAS_CMU1394CAMERA)
 ADD_LIBRARY(vvid ${vvid_sources})
ENDIF(HAS_CMU1394CAMERA)


LINK_LIBRARIES(vvid vpro strk sdet gevd vtol brip bgui vgui vidl_vil1 vbl vul vil1 bdgl)


# the main executables
SUBDIRS(bin)

ENDIF(HAS_VGUI)
