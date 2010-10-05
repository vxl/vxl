# brcv/mvl/ihog/exe/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR}/vpgl )

SET( register_images_exe_sources
        register_images_exe.cxx
   )

ADD_EXECUTABLE( register_images_exe  ${register_images_exe_sources})
TARGET_LINK_LIBRARIES(register_images_exe ihog bsta vnl vbl vul vil)

ADD_EXECUTABLE (compute_homographies_exe compute_homographies_exe.cxx)
TARGET_LINK_LIBRARIES(compute_homographies_exe ihog vnl bsta vbl vul)
