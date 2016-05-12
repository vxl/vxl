# bbas/bpgl/ihog/exe/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${BRL_INCLUDE_DIR}/bseg )

set( register_images_exe_sources
     register_images_exe.cxx
   )

add_executable( register_images_exe  ${register_images_exe_sources})
target_link_libraries(register_images_exe ihog ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil)

add_executable(compute_homographies_exe compute_homographies_exe.cxx)
target_link_libraries(compute_homographies_exe ihog ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)
