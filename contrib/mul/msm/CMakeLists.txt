# mul/msm/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/msm
  DEPENDS contrib/mul/vimt core/vnl core/vsl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's SM Library"
)

set(msm_sources
  msm_add_all_loaders.h       msm_add_all_loaders.cxx
  msm_points.h                msm_points.cxx
  msm_ref_shape_model.h       msm_ref_shape_model.cxx
  msm_shape_model.h           msm_shape_model.cxx
  msm_shape_model_builder.h   msm_shape_model_builder.cxx
  msm_ref_shape_instance.h    msm_ref_shape_instance.cxx
  msm_shape_instance.h        msm_shape_instance.cxx
  msm_shape_perturber.h       msm_shape_perturber.cxx
  msm_wt_mat_2d.h             msm_wt_mat_2d.cxx

  msm_aligner.h               msm_aligner.cxx
  msm_translation_aligner.h   msm_translation_aligner.cxx
  msm_zoom_aligner.h          msm_zoom_aligner.cxx
  msm_similarity_aligner.h    msm_similarity_aligner.cxx
  msm_rigid_aligner.h         msm_rigid_aligner.cxx

  msm_curve.h                 msm_curve.cxx

  msm_param_limiter.h         msm_param_limiter.cxx
  msm_no_limiter.h            msm_no_limiter.cxx
  msm_box_limiter.h           msm_box_limiter.cxx
  msm_ellipsoid_limiter.h     msm_ellipsoid_limiter.cxx

  msm_reflect_shape.h         msm_reflect_shape.cxx
  msm_pose_maker.h            msm_pose_maker.cxx

  msm_cubic_bezier.h          msm_cubic_bezier.cxx
)

aux_source_directory(Templates msm_sources)
vxl_add_library( LIBNAME msm LIBSRCS ${msm_sources} )
target_link_libraries(msm mcal vimt)

add_subdirectory(utils)

if(BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
