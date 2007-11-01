# world modeling application
# contrib/bwm/algo/CMakeLists.txt

INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/UseBGUI3D.cmake )

IF(HAS_BGUI3D)

#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
#INCLUDE_DIRECTORIES( ${VXL_TIFF_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${VXL_GEOTIFF_INCLUDE_DIR} ) 

SET(bwm_algo_sources
   bwm_algo.cxx                   bwm_algo.h
   bwm_lidar_algo.cxx             bwm_lidar_algo.h
   bwm_utm_lat_lon.cxx            bwm_utm_lat_lon.h 
   bwm_rat_proj_camera.cxx        bwm_rat_proj_camera.h
   bwm_plane_fitting_lsf.cxx      bwm_plane_fitting_lsf.h
   bwm_soview2D_cross.cxx         bwm_soview2D_cross.h
   bwm_image_processor.cxx        bwm_image_processor.h
)


#ADD_EXECUTABLE( bwm_algo ${bwm_algo_sources} )
ADD_LIBRARY(bwm_algo ${bwm_algo_sources})

TARGET_LINK_LIBRARIES( bwm_algo bwm bmsh3d bmsh3d_algo bmsh3d_vis sdet vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgui bgeo)


ENDIF(HAS_BGUI3D)
