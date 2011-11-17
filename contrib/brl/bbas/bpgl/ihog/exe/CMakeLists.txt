# bbas/bpgl/ihog/exe/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET( register_images_exe_sources
     register_images_exe.cxx
   )

ADD_EXECUTABLE( register_images_exe  ${register_images_exe_sources})
TARGET_LINK_LIBRARIES(register_images_exe ihog vnl vbl vul vil)

ADD_EXECUTABLE (compute_homographies_exe compute_homographies_exe.cxx)
TARGET_LINK_LIBRARIES(compute_homographies_exe ihog vnl vbl vul)
