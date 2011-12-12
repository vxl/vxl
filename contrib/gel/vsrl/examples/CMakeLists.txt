# gel/vsrl/examples/CMakeLists.txt

LINK_LIBRARIES( vsrl vil1 )

IF( BUILD_RPL )

  ADD_EXECUTABLE( dense_matcher_full dense_matcher_full.cxx )
  ADD_EXECUTABLE( make_3d_output make_3d_output.cxx )

ENDIF( BUILD_RPL )
