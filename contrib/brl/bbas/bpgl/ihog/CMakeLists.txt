# bpgl/ihog/CMakeLists.txt
# image homography registration library

include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )


set(ihog_sources
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

aux_source_directory(Templates ihog_sources)

vxl_add_library(LIBRARY_NAME ihog LIBRARY_SOURCES  ${ihog_sources})

target_link_libraries(ihog ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)

if( VXL_BUILD_EXAMPLES )
#  add_subdirectory(examples)
endif()

# Executable library
add_subdirectory(exe)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
