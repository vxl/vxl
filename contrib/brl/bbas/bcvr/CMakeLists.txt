# Brown Elastic Curve Matching Library
# bbas/bcvr/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR}/bbas)
include_directories(${BRL_INCLUDE_DIR}/bseg)
include_directories(${GEL_INCLUDE_DIR})

set(bcvr_sources

bcvr_cvmatch.h
bcvr_cvmatch.cxx
bcvr_cvmatch_sptr.h

bcvr_clsd_cvmatch.h
bcvr_clsd_cvmatch.cxx
bcvr_clsd_cvmatch_sptr.h

bcvr_cv_cor.h
bcvr_cv_cor.cxx
bcvr_cv_cor_sptr.h

)

aux_source_directory(Templates bcvr_sources)

vxl_add_library(LIBRARY_NAME bcvr LIBRARY_SOURCES  ${bcvr_sources})

target_link_libraries(bcvr bsol vdgl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

