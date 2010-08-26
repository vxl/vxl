# vpgl/ihog/CMakeLists.txt
# Brown video registration library
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl)
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(ihog_sources
   ihog_world_roi.cxx         ihog_world_roi.h          ihog_world_roi_sptr.h  
   ihog_region.cxx            ihog_region.h             ihog_region_sptr.h   
   ihog_transform_2d.cxx      ihog_transform_2d.h       ihog_transform_2d_sptr.h     
   ihog_cost_func.cxx         ihog_cost_func.h  
   ihog_minimizer.cxx         ihog_minimizer.h 
   ihog_lsqr_cost_func.cxx    ihog_lsqr_cost_func.h 
   ihog_super_res.cxx         ihog_super_res.h
   ihog_sample_grid_bilin.cxx ihog_sample_grid_bilin.h
   ihog_image.h
)

AUX_SOURCE_DIRECTORY(Templates ihog_sources)

ADD_LIBRARY(ihog  ${ihog_sources})

TARGET_LINK_LIBRARIES(ihog vnl vil vbl vgl_io vgl_algo vil_algo vsl)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

# Executable library
SUBDIRS(exe)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
