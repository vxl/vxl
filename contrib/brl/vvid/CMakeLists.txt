#this-sets-shell-mode-in-emacs-*-sh-*-
# ./brl/vvid/CMakeLists.txt
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/brl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )
# find the path to the cmu camera libraries and includes
INCLUDE(${allvxl_SOURCE_DIR}/brl/vvid/find_cmu_camera.cmake)

# determines if vgui is built and opengl is available
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/UseVGUI.cmake )
 
IF(HAS_VGUI)
# we have vgui enabled so ..
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/oxl )

# Uses the vidl_avicodec, and thus will only build under Windows
IF(WIN32)
IF(NOT CYGWIN)

# sources that don't depend on the camera being present
SOURCE_FILES(vvid_sources vvid_file_manager vvid_video_process vvid_epipolar_space_process) 

# sources that do depend on the camera being present
SOURCE_FILES(cmu_camera_sources cmu_1394_camera_params cmu_1394_camera vvid_live_video_frame vvid_live_video_manager)
AUX_SOURCE_DIRECTORY(Templates cmu_camera_sources)

# check if we have the camera libraries and assemble accordingly
IF(HAS_CMU1394CAMERA)
ADD_LIBRARY(vvid vvid_sources cmu_camera_sources)
ELSE(HAS_CMU1394CAMERA)
ADD_LIBRARY(vvid vvid_sources)
ENDIF(HAS_CMU1394CAMERA)

TARGET_LINK_LIBRARIES( vvid gevd vgui vidl vul vil )

# the main executables
SUBDIRS(bin)

ENDIF (NOT CYGWIN)
ENDIF(WIN32)
ENDIF(HAS_VGUI)
