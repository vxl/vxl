# brl/vvid/CMakeLists.txt

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/brl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )
# find the path to the cmu camera libraries and includes
INCLUDE(${allvxl_SOURCE_DIR}/brl/vvid/find_cmu_camera.cmake)

# determines if vgui is built and opengl is available
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/UseVGUI.cmake )
 
IF(HAS_VGUI)
# we have vgui enabled so ..
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/oxl )

# sources that don't depend on the camera being present
SOURCE_FILES(vvid_sources
             vvid_file_manager.cxx vvid_video_process.cxx vvid_epipolar_space_process.cxx
             vvid_file_manager.h   vvid_video_process.h   vvid_epipolar_space_process.h
                                   vvid_video_process_sptr.h )

# sources that do depend on the camera being present
SOURCE_FILES(cmu_camera_sources cmu_1394_camera_params.cxx cmu_1394_camera_params.h )
                                       
IF(HAS_CMU1394CAMERA)
 ADD_DEFINITIONS(-DHAS_CMU1394CAMERA)
 SOURCE_FILES(cmu_camera_sources
              cmu_1394_camera.cxx vvid_live_video_frame.cxx vvid_live_video_manager.cxx
              cmu_1394_camera.h   vvid_live_video_frame.h   vvid_live_video_manager.h
                                                            vvid_live_video_frame_sptr.h )
 AUX_SOURCE_DIRECTORY(Templates cmu_camera_sources)
ENDIF(HAS_CMU1394CAMERA)

# check if we have the camera libraries and assemble accordingly
IF(HAS_CMU1394CAMERA)
ADD_LIBRARY(vvid vvid_sources cmu_camera_sources)
ELSE(HAS_CMU1394CAMERA)
ADD_LIBRARY(vvid vvid_sources)
ENDIF(HAS_CMU1394CAMERA)

TARGET_LINK_LIBRARIES( vvid gevd vgui vidl vul vil )

# the main executables
SUBDIRS(bin)

ENDIF(HAS_VGUI)
