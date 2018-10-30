# This is /brl/bbas/bgui3d

doxygen_add_library(contrib/brl/bbas/bgui3d
  DEPENDS core/vgui contrib/gel/mrc/vpgl core/vnl
  PACKAGE contrib/brl
  DESCRIPTION "Brown 3D VGUI Extention Library (using Coin3D)"
 )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

# determines if vgui is built and opengl is available
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

set(HAS_BGUI3D "NO")
if(VXL_BUILD_BGUI3D)

  # Find the Coin3D library
  find_package(Coin3D)

  if(COIN3D_FOUND)
    include_directories( ${COIN3D_INCLUDE_DIRS} )
    set( HAS_BGUI3D "YES" )
    set(COIN3D_FOUND "YES")
    add_definitions( -DHAS_BGUI3D )
  endif()

  # Find the SimVoleon library
  include( ${VXL_CMAKE_DIR}/NewCMake/FindSIMVoleon.cmake )

  if(SIMVOLEON_FOUND)
    include_directories( ${SIMVOLEON_INCLUDE_DIR} )
  endif()

endif()

# Flag that determines if we were able to successfully build bgui3d.
# Initialize to NO. Change below if yes.
set(BGUI3D_FOUND "NO" CACHE INTERNAL "Was bgui3d successfully built?" )

# If Coin3d was found and building bgui3d
if(HAS_BGUI3D)
set(bgui3d_sources
    bgui3d.cxx                      bgui3d.h
    bgui3d_translate_event.cxx      bgui3d_translate_event.h
    bgui3d_file_io.cxx              bgui3d_file_io.h
    bgui3d_algo.cxx                 bgui3d_algo.h

    bgui3d_tableau.cxx              bgui3d_tableau.h              bgui3d_tableau_sptr.h
    bgui3d_viewer_tableau.cxx       bgui3d_viewer_tableau.h       bgui3d_viewer_tableau_sptr.h
    bgui3d_fullviewer_tableau.cxx   bgui3d_fullviewer_tableau.h   bgui3d_fullviewer_tableau_sptr.h
    bgui3d_examiner_tableau.cxx     bgui3d_examiner_tableau.h     bgui3d_examiner_tableau_sptr.h
    bgui3d_project2d_tableau.cxx    bgui3d_project2d_tableau.h    bgui3d_project2d_tableau_sptr.h
    bgui3d_examiner_slider_tableau.cxx    bgui3d_examiner_slider_tableau.h    bgui3d_examiner_slider_tableau_sptr.h
   )

vxl_add_library(LIBRARY_NAME bgui3d LIBRARY_SOURCES  ${bgui3d_sources})
set(BGUI3D_FOUND "YES" CACHE INTERNAL "Was bgui3d successfully built?" )
target_link_libraries(bgui3d ${COIN3D_LIBRARIES} ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul)

if(SIMVOLEON_FOUND)
  target_link_libraries(bgui3d ${SIMVOLEON_LIBRARY})
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

endif()
