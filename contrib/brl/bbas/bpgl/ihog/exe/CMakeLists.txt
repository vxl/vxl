# bbas/bpgl/ihog/exe/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXLCORE_INCLUDE_DIR} )

set( register_images_exe_sources
     register_images_exe.cxx
   )

add_executable( register_images_exe  ${register_images_exe_sources})
target_link_libraries(register_images_exe ihog vnl vbl vul vil)

add_executable(compute_homographies_exe compute_homographies_exe.cxx)
target_link_libraries(compute_homographies_exe ihog vnl vbl vul)
