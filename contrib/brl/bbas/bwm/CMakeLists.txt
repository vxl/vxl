# world modeling application
# brl/bbas/bwm/CMakeLists.txt




PROJECT(bwm)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl) 
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
IF(DIRECTSHOW_FOUND)
INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIRS})
ENDIF(DIRECTSHOW_FOUND)

INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
ENDIF(TIFF_FOUND)

INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
IF(GEOTIFF_FOUND)
  INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )
ENDIF(GEOTIFF_FOUND)

INCLUDE( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

# Set CMake options for using vgui
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
IF(HAS_VGUI)

SET(bwm_sources
   bwm_macros.h
   bwm_def.h
   bwm_input.h
   bwm_command.h                    bwm_command_sptr.h
                                    bwm_tableau.h                    bwm_tableau_sptr.h
   bwm_tableau_text.cxx             bwm_tableau_text.h
   bwm_observable.h                 bwm_observable_sptr.h
   bwm_observable_polyline.cxx      bwm_observable_polyline.h         bwm_observable_polyline_sptr.h
   bwm_observable_point.cxx         bwm_observable_point.h            bwm_observable_point_sptr.h
   bwm_observer.cxx                 bwm_observer.h
   bwm_plane_fitting_lsf.cxx        bwm_plane_fitting_lsf.h  # moved from algo to fix cyclic dependency
)

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  IF(SHAPELIB_FOUND)
    ADD_DEFINITIONS( -DHAS_SHAPELIB=1 )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/shapelib)
    SET(bwm_sources ${bwm_sources}
       bwm_command_macros.h
       bwm_load_commands.h
       bwm_process_commands.h
       bwm_process_mgr.cxx              bwm_process_mgr.h
       bwm_popup_menu.cxx               bwm_popup_menu.h
       bwm_observer_img.cxx             bwm_observer_img.h
       bwm_menu.cxx                     bwm_menu.h
       bwm_world.cxx                    bwm_world.h
       bwm_site_mgr.cxx                 bwm_site_mgr.h
       bwm_tableau_cam.cxx              bwm_tableau_cam.h
       bwm_tableau_mgr.cxx              bwm_tableau_mgr.h
       bwm_tableau_img.cxx              bwm_tableau_img.h
       bwm_tableau_factory.cxx          bwm_tableau_factory.h
       bwm_tableau_proj_cam.cxx         bwm_tableau_proj_cam.h
       bwm_tableau_geo_cam.cxx			bwm_tableau_geo_cam.h
	   bwm_tableau_rat_cam.cxx          bwm_tableau_rat_cam.h
       bwm_tableau_generic_cam.cxx      bwm_tableau_generic_cam.h
       bwm_tableau_video.cxx            bwm_tableau_video.h
       bwm_observable_mesh.cxx          bwm_observable_mesh.h             bwm_observable_mesh_sptr.h
       bwm_observer_cam.cxx             bwm_observer_cam.h
       bwm_observer_vgui.cxx            bwm_observer_vgui.h
       bwm_observer_video.cxx           bwm_observer_video.h
       bwm_observer_mgr.cxx             bwm_observer_mgr.h
       bwm_observer_geo_cam.cxx        bwm_observer_geo_cam.h

       bwm_observer_proj_cam.cxx        bwm_observer_proj_cam.h
       bwm_observer_rat_cam.cxx         bwm_observer_rat_cam.h
       bwm_observer_generic_cam.cxx     bwm_observer_generic_cam.h
       bwm_observable_mesh_circular.cxx bwm_observable_mesh_circular.h
       bwm_observable_textured_mesh.cxx bwm_observable_textured_mesh.h    bwm_observable_textured_mesh_sptr.h
       bwm_texture_map_generator.cxx    bwm_texture_map_generator.h
       bwm_corr.cxx                     bwm_corr.h          bwm_corr_sptr.h
       bwm_3d_corr.cxx                  bwm_3d_corr.h       bwm_3d_corr_sptr.h
    )
  ENDIF(SHAPELIB_FOUND)
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

IF(NOT MINGW)
  # MFC is not available in MinGW.
  # This should be fixed in FindMFC.cmake and then this IF can be removed
  INCLUDE(${CMAKE_ROOT}/Modules/FindMFC.cmake)
ENDIF(NOT MINGW)

IF(MFC_FOUND)
  # add stuff to use MFC in this executable
  ADD_DEFINITIONS( "-D_AFXDLL" )
  ADD_DEFINITIONS( -DHAS_MFC=1 )
  SET(CMAKE_MFC_FLAG 2)
  SET(bwm_sources ${bwm_sources}
     process/bwm_process.cxx           process/bwm_process.h
     process/bwm_site_process.cxx      process/bwm_site_process.h
  )
ENDIF(MFC_FOUND)

AUX_SOURCE_DIRECTORY(Templates bwm_sources)

ADD_LIBRARY(bwm ${bwm_sources})

TARGET_LINK_LIBRARIES( bwm bwm_video bwm_reg bwm_algo bwm_io vsph depth_map bsta_io bmsh3d bmsh3d_algo bsol vsol vdgl bgui vgui vil vbl vgl vgl_algo vnl_algo vnl vpgl vpgl_algo vpgl_io vpgl_file_formats bvgl vul volm)

SUBDIRS(algo)
SUBDIRS(pro)
SUBDIRS(io)
SUBDIRS(reg)


#install the .h .txx and libs
INSTALL_TARGETS(/lib bwm)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/ ${bwm_sources})

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  IF(WIN32)
   SUBDIRS(wxWidgets)
  ENDIF(WIN32)
  IF(SHAPELIB_FOUND)
   SUBDIRS(exe)
   IF( BUILD_TESTING )
    SUBDIRS(tests)
   ENDIF( BUILD_TESTING )
  ENDIF(SHAPELIB_FOUND)
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

ENDIF(HAS_VGUI)


SUBDIRS(video)
