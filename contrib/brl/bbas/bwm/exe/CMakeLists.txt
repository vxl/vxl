# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables
#INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( bwm_main bwm_main.cxx )
TARGET_LINK_LIBRARIES( bwm_main bwm vgui vil )

ADD_EXECUTABLE( bwm_img_main bwm_img_main.cxx )
TARGET_LINK_LIBRARIES( bwm_img_main bwm vgui )
