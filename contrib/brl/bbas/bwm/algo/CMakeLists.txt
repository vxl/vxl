# world modeling application
# contrib/bwm/algo/CMakeLists.txt

INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/UseBGUI3D.cmake )

IF(HAS_BGUI3D)

SET(bwm_algo_sources
   bwm_algo.cxx                   bwm_algo.h
   bwm_lidar_algo.cxx             bwm_lidar_algo.h
   bwm_utm_lat_lon.cxx            bwm_utm_lat_lon.h 
   bwm_rat_proj_camera.cxx        bwm_rat_proj_camera.h
   bwm_plane_fitting_lsf.cxx      bwm_plane_fitting_lsf.h
   bwm_soview2D_cross.cxx         bwm_soview2D_cross.h
                                  bwm_soview2D_vertex.h
   bwm_image_processor.cxx        bwm_image_processor.h
   bwm_utils.cxx                  bwm_utils.h
#   bwm_backgroundTaskDialog.cxx   bwm_backgroundTaskDialog.h	
#   bwm_site_creation_process.cxx  bwm_site_creation_process.h	
#   bwm_site.h	
#   StdAfx.cpp                       StdAfx.h
)


#ADD_EXECUTABLE( bwm_algo ${bwm_algo_sources} )
ADD_LIBRARY(bwm_algo ${bwm_algo_sources})

TARGET_LINK_LIBRARIES( bwm_algo bwm bmsh3d bmsh3d_algo bmsh3d_vis sdet vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgui bgeo)

ENDIF(HAS_BGUI3D)


