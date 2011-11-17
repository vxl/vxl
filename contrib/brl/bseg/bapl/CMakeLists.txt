# Brown affine patch library
# contrib/brl/bseg/bapl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})

SET(bapl_sources
   bapl_keypoint.cxx            bapl_keypoint.h                 bapl_keypoint_sptr.h
   bapl_keypoint_set.cxx        bapl_keypoint_set.h             bapl_keypoint_set_sptr.h
   bapl_lowe_keypoint.cxx       bapl_lowe_keypoint.h            bapl_lowe_keypoint_sptr.h
   bapl_lowe_pyramid.txx        bapl_lowe_pyramid.h
   bapl_lowe_pyramid_set.cxx    bapl_lowe_pyramid_set.h         bapl_lowe_pyramid_set_sptr.h
   bapl_keypoint_extractor.cxx  bapl_keypoint_extractor.h
   bapl_bbf_tree.cxx            bapl_bbf_tree.h
   bapl_lowe_cluster.cxx        bapl_lowe_cluster.h
   bapl_affine2d_est.cxx        bapl_affine2d_est.h
   bapl_affine_transform.h      bapl_affine_transform.cxx
   bapl_affine_roi.h            bapl_affine_roi.cxx
   bapl_mi_matcher.h            bapl_mi_matcher.cxx
   bapl_mi_matcher_params.h     bapl_mi_matcher_params.cxx
   bapl_connectivity.h          bapl_connectivity.cxx           bapl_connectivity_sptr.h
   bapl_dense_sift.h            bapl_dense_sift.cxx             bapl_dense_sift_sptr.h
   bapl_dsift.h                 bapl_dsift.cxx                  bapl_dsift_sptr.h
   )
AUX_SOURCE_DIRECTORY(Templates bapl_sources)

ADD_LIBRARY(bapl  ${bapl_sources})
TARGET_LINK_LIBRARIES(bapl bpgl_algo vpgl_algo ipts vimt brip rrel vnl_algo vnl vil_algo vil_io vil vgl_algo vgl vbl_io vbl)

#IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(pro)

SUBDIRS(io)

