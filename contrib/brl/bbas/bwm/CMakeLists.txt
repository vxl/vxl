# world modeling application
# contrib/brl/bwm/CMakeLists.txt

INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/UseBGUI3D.cmake )

IF(HAS_BGUI3D)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/tiff )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/geotiff ) 

SET(bwm_sources
   bwm_macros.h
   bwm_menu.cxx                bwm_menu.h
   bwm_command.h               bwm_command_sptr.h   
   bwm_command_macros.h
   bwm_load_commands.h
   bwm_process_commands.h
   bwm_tableau_cam.cxx         bwm_tableau_cam.h
   bwm_tableau_rat_cam.cxx     bwm_tableau_rat_cam.h
   bwm_tableau_proj_cam.cxx    bwm_tableau_proj_cam.h
   bwm_tableau_coin3d.cxx      bwm_tableau_coin3d.h
                               bwm_tableau_proj2d.h
   bwm_tableau_lidar.cxx       bwm_tableau_lidar.h
   bwm_tableau_text.cxx        bwm_tableau_text.h
   bwm_tableau_img.cxx         bwm_tableau_img.h
   bwm_tableau_mgr.cxx         bwm_tableau_mgr.h
   bwm_process_mgr.cxx         bwm_process_mgr.h
   bwm_observable.h            bwm_observable_sptr.h
   bwm_observable_mesh.cxx     bwm_observable_mesh.h       bwm_observable_mesh_sptr.h
   bwm_observable_textured_mesh.cxx     bwm_observable_textured_mesh.h bwm_observable_textured_mesh_sptr.h
   bwm_observable_polyline.cxx bwm_observable_polyline.h   bwm_observable_polyline_sptr.h 
   bwm_observable_point.cxx    bwm_observable_point.h      bwm_observable_point.h
   bwm_observer.cxx            bwm_observer.h         
   bwm_observer_img.cxx        bwm_observer_img.h
   bwm_observer_vgui.cxx       bwm_observer_vgui.h  
   bwm_observer_cam.cxx        bwm_observer_cam.h     
   bwm_observer_proj_cam.cxx   bwm_observer_proj_cam.h
   bwm_observer_rat_cam.cxx    bwm_observer_rat_cam.h
   bwm_observer_coin3d.cxx     bwm_observer_coin3d.h
                               bwm_observer_proj2d.h
   bwm_observer_lidar.cxx      bwm_observer_lidar.h
   bwm_observer_mgr.cxx        bwm_observer_mgr.h    
   bwm_texture_map_generator.cxx bwm_texture_map_generator.h
   bwm_corr.cxx                bwm_corr.h       bwm_corr_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_sources)

ADD_LIBRARY(bwm ${bwm_sources})

TARGET_LINK_LIBRARIES( bwm bwm_algo bmsh3d bmsh3d_algo bmsh3d_vis vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgui bgeo)

SUBDIRS(exe)

SUBDIRS(algo)

SUBDIRS(tests)

ENDIF(HAS_BGUI3D)
