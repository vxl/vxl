# bpgl/ihog/CMakeLists.txt
# image homography registration library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )


SET(ihog_sources
   ihog_world_roi.cxx         ihog_world_roi.h          ihog_world_roi_sptr.h
   ihog_transform_2d.cxx      ihog_transform_2d.h       ihog_transform_2d_sptr.h
   ihog_minimizer.cxx         ihog_minimizer.h
   ihog_lsqr_cost_func.cxx    ihog_lsqr_cost_func.h
   ihog_sample_grid_bilin.cxx ihog_sample_grid_bilin.h
                              ihog_image.h
   ihog_utils.cxx             ihog_utils.h
   ihog_cost_func.cxx         ihog_cost_func.h
   ihog_minfo_cost_func.cxx   ihog_minfo_cost_func.h
)

AUX_SOURCE_DIRECTORY(Templates ihog_sources)

ADD_LIBRARY(ihog  ${ihog_sources})

TARGET_LINK_LIBRARIES(ihog vnl_algo vnl vil vgl_io vgl_algo vgl vil_algo vbl_io vbl vsl vul)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

# Executable library
SUBDIRS(exe)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
