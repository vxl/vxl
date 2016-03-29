# contrib/brl/bbas/bil/CMakeLists.txt
# Brown Image Library


set(bil_sources
    bil_math.h
    bil_bounded_image_view.h    bil_bounded_image_view.hxx
    bil_raw_image_istream.h     bil_raw_image_istream.cxx
        bil_arf_image_istream.h     bil_arf_image_istream.cxx
   )

aux_source_directory(Templates bil_sources)

vxl_add_library(LIBRARY_NAME bil LIBRARY_SOURCES  ${bil_sources})

target_link_libraries(bil ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vsl)

add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(exe)