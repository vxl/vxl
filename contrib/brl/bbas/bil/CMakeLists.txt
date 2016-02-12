# contrib/brl/bbas/bil/CMakeLists.txt
# Brown Image Library


set(bil_sources
    bil_math.h
    bil_bounded_image_view.h    bil_bounded_image_view.txx
    bil_raw_image_istream.h     bil_raw_image_istream.cxx
        bil_arf_image_istream.h     bil_arf_image_istream.cxx
   )

aux_source_directory(Templates bil_sources)

add_library(bil  ${bil_sources})

target_link_libraries(bil ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vsl)

add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(exe)

install_targets(/lib bil)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bil ${bil_sources})
