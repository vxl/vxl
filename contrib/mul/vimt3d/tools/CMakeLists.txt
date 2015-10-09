# mul/vimt3d/tools/CMakeLists.txt
#
#  Tim Cootes
#
LINK_LIBRARIES(vimt3d vnl vil vul)

ADD_EXECUTABLE(vimt3d_project_image vimt3d_project_image.cxx)
ADD_EXECUTABLE(vimt3d_stack_slices vimt3d_stack_slices.cxx)
