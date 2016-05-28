# world modeling application
# brl/bbas/bwm/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(HAS_GEOTIFF)
include_directories( ${GEOTIFF_INCLUDE_DIR} )


project(bwm)

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl)
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR} )
include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
if(DIRECTSHOW_FOUND)
include_directories(${DIRECTSHOW_INCLUDE_DIRS})
endif()

include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
endif()


include( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

# Set CMake options for using vgui
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
if(HAS_VGUI)

set(bwm_sources
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

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)

  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
  if(SHAPELIB_FOUND)
    add_definitions( -DHAS_SHAPELIB=1 )
    include_directories( ${BRL_INCLUDE_DIR}/b3p/shapelib)
    set(bwm_sources ${bwm_sources}
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
       bwm_tableau_geo_cam.cxx          bwm_tableau_geo_cam.h
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
  endif()
 endif()
endif()

if(NOT MINGW)
  # MFC is not available in MinGW.
  # This should be fixed in FindMFC.cmake and then this IF can be removed
  find_package(MFC)
endif()

if(MFC_FOUND)
  # add stuff to use MFC in this executable
  add_definitions( "-D_AFXDLL" )
  add_definitions( -DHAS_MFC=1 )
  set(CMAKE_MFC_FLAG 2)
  set(bwm_sources ${bwm_sources}
     process/bwm_process.cxx           process/bwm_process.h
     process/bwm_site_process.cxx      process/bwm_site_process.h
  )
endif()

aux_source_directory(Templates bwm_sources)

vxl_add_library(LIBRARY_NAME bwm LIBRARY_SOURCES ${bwm_sources})

target_link_libraries( bwm bwm_video bwm_reg bwm_algo bwm_io vsph depth_map bsta_io bmsh3d bmsh3d_algo bsol vsol vdgl bgui ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_file_formats bvgl ${VXL_LIB_PREFIX}vul)

if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    target_link_libraries(bwm volm)
  endif()
endif()

add_subdirectory(algo)
add_subdirectory(pro)
add_subdirectory(io)
add_subdirectory(reg)


#install the .h .hxx and libs

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  if(WIN32)
   add_subdirectory(wxWidgets)
  endif()
  if(SHAPELIB_FOUND)
   add_subdirectory(exe)
   if( BUILD_TESTING )
    add_subdirectory(tests)
   endif()
  endif()
 endif()
endif()

endif()


add_subdirectory(video)
else()
  message(STATUS "Skipping contrib/brl/bbas/bwm: requires geotiff")
endif()
