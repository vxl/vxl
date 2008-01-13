# world modeling application
# contrib/brl/bwm/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/UseBGUI3D.cmake )

IF(HAS_BGUI3D)

IF(BUILD_EXPAT)
# Set CMake options for using vgui
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})

INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
IF(GEOTIFF_FOUND)
  INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )

SET(bwm_sources
   bwm_macros.h
   bwm_def.h
   bwm_input.h
   bwm_menu.cxx                bwm_menu.h
   bwm_popup_menu.cxx          bwm_popup_menu.h
   bwm_command.h               bwm_command_sptr.h
   bwm_command_macros.h
   bwm_load_commands.h
   bwm_process_commands.h
   bwm_site.cxx                bwm_site.h               bwm_site_sptr.h
   bwm_world.cxx               bwm_world.h
                               bwm_tableau.h            bwm_tableau_sptr.h
   bwm_tableau_cam.cxx         bwm_tableau_cam.h
   bwm_tableau_rat_cam.cxx     bwm_tableau_rat_cam.h
   bwm_tableau_proj_cam.cxx    bwm_tableau_proj_cam.h
#   bwm_tableau_coin3d.cxx
                               bwm_tableau_coin3d.h
                               bwm_tableau_proj2d.h
#   bwm_tableau_lidar.cxx
                               bwm_tableau_lidar.h
   bwm_tableau_text.cxx        bwm_tableau_text.h
   bwm_tableau_img.cxx         bwm_tableau_img.h
    bwm_tableau_video.cxx     bwm_tableau_video.h
   bwm_tableau_mgr.cxx         bwm_tableau_mgr.h
   bwm_process_mgr.cxx         bwm_process_mgr.h
   bwm_observable.h            bwm_observable_sptr.h
   bwm_mesh_observable.h
   bwm_observable_mesh.cxx     bwm_observable_mesh.h       bwm_observable_mesh_sptr.h
   bwm_observable_textured_mesh.cxx bwm_observable_textured_mesh.h bwm_observable_textured_mesh_sptr.h
   bwm_observable_polyline.cxx bwm_observable_polyline.h   bwm_observable_polyline_sptr.h
   bwm_observable_point.cxx    bwm_observable_point.h      bwm_observable_point_sptr.h
   bwm_observer.cxx            bwm_observer.h
   bwm_observer_img.cxx        bwm_observer_img.h
   bwm_observer_vgui.cxx       bwm_observer_vgui.h
   bwm_observer_cam.cxx        bwm_observer_cam.h
   bwm_observer_proj_cam.cxx   bwm_observer_proj_cam.h
   bwm_observer_cam_proj.h
   bwm_observer_rat_cam.cxx    bwm_observer_rat_cam.h
   bwm_observer_cam_rat.h
#   bwm_observer_coin3d.cxx
                               bwm_observer_coin3d.h
                               bwm_observer_proj2d.h
                               bwm_observer_proj2d_simple.h
#   bwm_observer_lidar.cxx
                               bwm_observer_lidar.h
    bwm_observer_video.cxx     bwm_observer_video.h
   bwm_observer_mgr.cxx        bwm_observer_mgr.h
   bwm_texture_map_generator.cxx bwm_texture_map_generator.h
   bwm_corr.cxx                bwm_corr.h       bwm_corr_sptr.h
)

IF(NOT MINGW)
  # MFC is not available in MinGW.
  # This should be fixed in FindMFC.cmake and then this IF can be removed
  INCLUDE(${CMAKE_ROOT}/Modules/FindMFC.cmake)
ENDIF(NOT MINGW)

IF (MFC_FOUND)
  # add stuff to use MFC in this executable
  ADD_DEFINITIONS( "-D_AFXDLL" )
  SET(CMAKE_MFC_FLAG 2)


SET(bwm_sources ${bwm_sources}
   process/bwm_process.cxx           process/bwm_process.h
   process/bwm_site_process.cxx      process/bwm_site_process.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_sources)

ADD_LIBRARY(bwm ${bwm_sources})

TARGET_LINK_LIBRARIES( bwm bwm_video bwm_reg bwm_algo bwm_io bmsh3d bmsh3d_algo bmsh3d_vis expatpp vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo vpgl_file_formats bgui bgeo)

SUBDIRS(exe)
SUBDIRS(algo)
SUBDIRS(io)
SUBDIRS(reg)
SUBDIRS(video)
SUBDIRS(tests)
ENDIF (MFC_FOUND)
ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)
ENDIF(BUILD_EXPAT)
ENDIF(HAS_BGUI3D)
