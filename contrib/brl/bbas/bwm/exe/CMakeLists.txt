# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

ADD_EXECUTABLE( bwm_main bwm_main.cxx)
TARGET_LINK_LIBRARIES( bwm_main bwm bwm_io bwm_reg vgui vil)

ADD_EXECUTABLE( bwm_batch_project bwm_batch_project.cxx)
TARGET_LINK_LIBRARIES( bwm_batch_project bwm)



