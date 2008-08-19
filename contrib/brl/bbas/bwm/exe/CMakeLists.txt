# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

ADD_EXECUTABLE( bwm_main bwm_main.cxx bwm_resources.res)
TARGET_LINK_LIBRARIES( bwm_main bwm bwm_io bwm_reg vgui vil)


