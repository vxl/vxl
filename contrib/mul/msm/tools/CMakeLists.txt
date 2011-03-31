# mul/msm/tools/CMakeLists.txt
#
#  Tim Cootes
#

LINK_LIBRARIES(msm msm_utils vil vgl vul)

ADD_EXECUTABLE(msm_build_shape_model msm_build_shape_model.cxx)
ADD_EXECUTABLE(msm_draw_shape_modes  msm_draw_shape_modes.cxx)
ADD_EXECUTABLE(msm_draw_points_on_image  msm_draw_points_on_image.cxx)
ADD_EXECUTABLE(msm_apply_tps_warp msm_apply_tps_warp.cxx)
ADD_EXECUTABLE(msm_plot_aligned_shapes msm_plot_aligned_shapes.cxx)
ADD_EXECUTABLE(msm_get_shape_params msm_get_shape_params.cxx)
