# mul/msm/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/msm
  DEPENDS contrib/mul/vimt core/vnl core/vsl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's SM Library"
)

SET(msm_sources
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
)

AUX_SOURCE_DIRECTORY(Templates msm_sources)
ADD_LIBRARY(msm ${msm_sources} )
TARGET_LINK_LIBRARIES(msm mcal vimt)

SUBDIRS(utils)

IF (BUILD_MUL_TOOLS)
  SUBDIRS(tools)
ENDIF (BUILD_MUL_TOOLS)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
