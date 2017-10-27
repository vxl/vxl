# brl/bbas/bseg/sdet/tests/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bbas)

add_executable(sdet_test_all
  test_driver.cxx
  test_detector.cxx
  test_region_proc.cxx
  test_gauss_fit.cxx
  test_denoise_mrf.cxx
  test_denoise_mrf_bp.cxx
  test_segmentation.cxx
  test_selective_search.cxx
  test_region_classifier.cxx
  test_sel.cxx
)
target_link_libraries(sdet_test_all sdet imesh_algo vtol ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib)

add_test( NAME sdet_test_detector COMMAND $<TARGET_FILE:sdet_test_all> test_detector    ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
add_test( NAME sdet_test_region_proc COMMAND $<TARGET_FILE:sdet_test_all> test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
add_test( NAME sdet_test_gauss_fit COMMAND $<TARGET_FILE:sdet_test_all> test_gauss_fit )
add_test( NAME sdet_test_denoise_mrf COMMAND $<TARGET_FILE:sdet_test_all> test_denoise_mrf )
add_test( NAME sdet_test_denoise_mrf_bp COMMAND $<TARGET_FILE:sdet_test_all> test_denoise_mrf_bp )
add_test( NAME sdet_test_segmentation COMMAND $<TARGET_FILE:sdet_test_all> test_segmentation )
add_test( NAME sdet_test_selective_search COMMAND $<TARGET_FILE:sdet_test_all> test_selective_search )
add_test( NAME sdet_test_region_classifier COMMAND $<TARGET_FILE:sdet_test_all> test_region_classifier )
add_test( NAME sdet_test_sel COMMAND $<TARGET_FILE:sdet_test_all> test_sel)
add_executable(sdet_test_include test_include.cxx)
target_link_libraries(sdet_test_include sdet)
