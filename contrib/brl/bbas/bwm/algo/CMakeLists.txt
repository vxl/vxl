# world modeling application
# brl/bbas/bwm/algo/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

SET(bwm_algo_sources
    bwm_algo.cxx                   bwm_algo.h
    bwm_lidar_algo.cxx             bwm_lidar_algo.h
    bwm_utm_lat_lon.cxx            bwm_utm_lat_lon.h
    bwm_rat_proj_camera.cxx        bwm_rat_proj_camera.h
    bwm_soview2D_cross.cxx         bwm_soview2D_cross.h
                                   bwm_soview2D_vertex.h
    bwm_image_processor.cxx        bwm_image_processor.h
    bwm_delaunay_tri.cxx           bwm_delaunay_tri.h
   )

IF(SHAPELIB_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/shapelib)
  SET(bwm_algo_sources             ${bwm_algo_sources}
      bwm_shape_file.cxx             bwm_shape_file.h
      bwm_utils.cxx                  bwm_utils.h
      bwm_plane_fitting_lsf.cxx      bwm_plane_fitting_lsf.h
     )
  ADD_DEFINITIONS( -DHAS_SHAPELIB=1 )
ENDIF(SHAPELIB_FOUND)

# ERMAPPER JPEG2000 COMPRESSION SDK
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
IF(ECW_FOUND)
  INCLUDE_DIRECTORIES(${ECW_INCLUDE_DIR})
ENDIF(ECW_FOUND)

ADD_LIBRARY(bwm_algo ${bwm_algo_sources})

TARGET_LINK_LIBRARIES( bwm_algo bwm sdet vtol vil vil_algo vgl vgl_algo vnl vnl_algo vsol vpgl vpgl_algo vpgl_file_formats bgui brip vdgl bsta vgui vul vcl shapelib )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
