# world modeling application
# brl/bbas/bwm/algo/CMakeLists.txt

include( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

set(bwm_algo_sources
    bwm_algo.cxx                   bwm_algo.h
    bwm_lidar_algo.cxx             bwm_lidar_algo.h
    bwm_utm_lat_lon.cxx            bwm_utm_lat_lon.h
    bwm_rat_proj_camera.cxx        bwm_rat_proj_camera.h
    bwm_soview2D_cross.cxx         bwm_soview2D_cross.h
                                   bwm_soview2D_vertex.h
    bwm_image_processor.cxx        bwm_image_processor.h
    bwm_delaunay_tri.cxx           bwm_delaunay_tri.h
   )

aux_source_directory(Templates bwm_algo_sources)

if(SHAPELIB_FOUND)

include_directories( ${BRL_INCLUDE_DIR}/b3p/shapelib)
  set(bwm_algo_sources             ${bwm_algo_sources}
      bwm_shape_file.cxx             bwm_shape_file.h
      bwm_utils.cxx                  bwm_utils.h
     )
  add_definitions( -DHAS_SHAPELIB=1 )
endif(SHAPELIB_FOUND)

# ERMAPPER JPEG2000 COMPRESSION SDK
include( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
if(ECW_FOUND)
  include_directories(${ECW_INCLUDE_DIR})
endif(ECW_FOUND)

add_library(bwm_algo ${bwm_algo_sources})

target_link_libraries( bwm_algo sdet vtol vil vil_algo vgl vgl_algo vnl vnl_algo vsol vpgl vpgl_algo vpgl_file_formats bgui brip vdgl bsta vgui vul vcl shapelib brad)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

#install the .h .txx and libs
install_targets(/lib bwm_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/algo/ ${bwm_algo_sources})
