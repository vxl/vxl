# brl/bexe/vded/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )


ADD_EXECUTABLE (motion_test motion_test.cxx)
TARGET_LINK_LIBRARIES(motion_test vidl vil vil_algo vul)

ADD_EXECUTABLE (frame_extractor frame_extractor.cxx)
TARGET_LINK_LIBRARIES(frame_extractor vidl_vil1 vil1 vul)

ADD_EXECUTABLE (video_description video_description.cxx)
TARGET_LINK_LIBRARIES(video_description vidl_vil1 vil1 vul)
