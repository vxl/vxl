project(J2K)

if ( VXL_USE_ECW )
  include(${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake)

  if( ECW_FOUND )
    if( NOT VXL_USING_NATIVE_ECW )
      include_directories(${CMAKE_CURRENT_LIST_DIR}/Source/include)
      add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Source/C/NCSEcw/NCSEcw)
    endif()
  endif()

endif()
