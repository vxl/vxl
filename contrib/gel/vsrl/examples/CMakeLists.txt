# gel/vsrl/examples/CMakeLists.txt

LINK_LIBRARIES( vsrl vil1 )

IF( BUILD_RPL )

  ADD_EXECUTABLE( dense_matcher_full dense_matcher_full.cxx )
  ADD_EXECUTABLE( make_3d_output make_3d_output.cxx )

ENDIF( BUILD_RPL )

IF( HAS_VGUI )
IF( BUILD_TBL )
IF( BUILD_BRL )

  ADD_EXECUTABLE( vsrl_viewer vsrl_viewer.cxx )
  TARGET_LINK_LIBRARIES(vsrl_viewer vgui)

ENDIF( BUILD_BRL )
ENDIF( BUILD_TBL )
ENDIF( HAS_VGUI )
