# brl/vvid/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
# find the path to the cmu camera libraries and includes
INCLUDE(${BRL_INCLUDE_DIR}/bmods/find_cmu_camera.cmake)

# determines if vgui is built and opengl is available
INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )
 
IF(HAS_VGUI)
# we have vgui enabled so ..
INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )

# sources that don't depend on the camera being present
SET(vvid_sources
    vvid_video_process.cxx           vvid_video_process.h  vvid_video_process_sptr.h
    vvid_region_process.cxx          vvid_region_process.h   
    vvid_capture_process.cxx         vvid_capture_process.h   
    vvid_frame_diff_process.cxx      vvid_frame_diff_process.h   
    vvid_motion_process.cxx          vvid_motion_process.h   
    vvid_lucas_kanade_process.cxx    vvid_lucas_kanade_process.h   
    vvid_epipolar_space_process.cxx  vvid_epipolar_space_process.h   
    vvid_edge_process.cxx            vvid_edge_process.h   
    vvid_file_manager.cxx            vvid_file_manager.h
)

# sources that do depend on the camera being present
                                       
IF(HAS_CMU1394CAMERA)
 ADD_DEFINITIONS(-DHAS_CMU1394CAMERA)
 SET(cmu_camera_sources 
  cmu_1394_camera_params.cxx   cmu_1394_camera_params.h
  cmu_1394_camera.cxx          cmu_1394_camera.h
  vvid_live_video_tableau.cxx  vvid_live_video_tableau.h 
                               vvid_live_video_tableau_sptr.h     
  vvid_live_video_frame.cxx    vvid_live_video_frame.h vvid_live_video_frame_sptr.h
  vvid_live_video_manager.cxx  vvid_live_video_manager.h
  vvid_live_stereo_manager.cxx vvid_live_stereo_manager.h
 )
ENDIF(HAS_CMU1394CAMERA)

AUX_SOURCE_DIRECTORY(Templates vvid_sources)

# check if we have the camera libraries and assemble accordingly
IF(HAS_CMU1394CAMERA)
 ADD_LIBRARY(vvid ${vvid_sources} ${cmu_camera_sources})
ELSE(HAS_CMU1394CAMERA)
 ADD_LIBRARY(vvid ${vvid_sources})
ENDIF(HAS_CMU1394CAMERA)

LINK_LIBRARIES( vvid sdet gevd vtol brip bgui vgui vidl vbl vul vil )

# the main executables
SUBDIRS(bin)

ENDIF(HAS_VGUI)
