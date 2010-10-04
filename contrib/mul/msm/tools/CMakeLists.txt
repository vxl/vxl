#
# ./msm/tools/CMakeLists.txt
#
#  Tim Cootes
#

LINK_LIBRARIES(msm msm_utils)

ADD_EXECUTABLE(msm_build_shape_model msm_build_shape_model.cxx)
ADD_EXECUTABLE(msm_draw_shape_modes  msm_draw_shape_modes.cxx)
ADD_EXECUTABLE(msm_draw_points_on_image  msm_draw_points_on_image.cxx)
ADD_EXECUTABLE(msm_apply_tps_warp msm_apply_tps_warp.cxx)