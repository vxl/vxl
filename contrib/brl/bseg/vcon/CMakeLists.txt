# bseg/vcon/CMakeLists.txt
# VSI's Conflation Algorithms, Tools and Processes

include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(HAS_GEOTIFF)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
    include_directories( ${GEL_INCLUDE_DIR} )
    include_directories(${EXPAT_INCLUDE_DIRS} )
    include_directories(${EXPATPP_INCLUDE_DIRS} )

    add_subdirectory(pro)
  endif()
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/vcon: requires geotiff")
endif()
