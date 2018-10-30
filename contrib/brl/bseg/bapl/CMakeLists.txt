# Brown affine patch library
# contrib/brl/bseg/bapl/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${RPL_INCLUDE_DIR} )
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${GEL_INCLUDE_DIR})

set(bapl_sources
   bapl_keypoint.cxx            bapl_keypoint.h                 bapl_keypoint_sptr.h
   bapl_keypoint_set.cxx        bapl_keypoint_set.h             bapl_keypoint_set_sptr.h
   bapl_lowe_keypoint.cxx       bapl_lowe_keypoint.h            bapl_lowe_keypoint_sptr.h
   bapl_lowe_pyramid.hxx        bapl_lowe_pyramid.h
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
aux_source_directory(Templates bapl_sources)

vxl_add_library(LIBRARY_NAME bapl LIBRARY_SOURCES  ${bapl_sources})
target_link_libraries(bapl bpgl_algo ${VXL_LIB_PREFIX}vpgl_algo ipts vimt brip rrel ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl)

#if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
#endif()


if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(pro)

add_subdirectory(io)

