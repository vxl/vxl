# mul/vimt3d/tools/CMakeLists.txt
#
#  Tim Cootes
#
link_libraries(vimt3d ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul)

add_executable(vimt3d_project_image vimt3d_project_image.cxx)
add_executable(vimt3d_stack_slices vimt3d_stack_slices.cxx)
