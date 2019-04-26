# sdet/algo/CMakeLists.txt
# Segmentation Detection Algorithms library

set( sdet_algo_sources
  sdet_classify_clouds.h sdet_classify_clouds.cxx
)

vxl_add_library(LIBRARY_NAME sdet_algo LIBRARY_SOURCES ${sdet_algo_sources})

target_link_libraries(sdet_algo ${VXL_LIB_PREFIX}sdet ${VXL_LIB_PREFIX}vil)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

