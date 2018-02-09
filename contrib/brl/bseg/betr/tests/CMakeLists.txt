# brl/bseg/betr/tests/CMakeLists.txt
include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bbas)

add_executable(betr_test_all
  test_driver.cxx
  test_geo_box.cxx
  test_geo_object.cxx
  test_site.cxx
  test_edgel_factory.cxx
  test_event_trigger.cxx
  test_edgel_change_detection.cxx
  test_json_params.cxx
)
target_link_libraries(betr_test_all betr ${VXL_LIB_PREFIX}testlib)

add_test( NAME betr_test_geo_box COMMAND $<TARGET_FILE:betr_test_all> test_geo_box )
add_test( NAME betr_test_geo_object COMMAND $<TARGET_FILE:betr_test_all> test_geo_object )
add_test( NAME betr_test_site COMMAND $<TARGET_FILE:betr_test_all> test_site )
add_test( NAME betr_test_edgel_factory COMMAND $<TARGET_FILE:betr_test_all> test_edgel_factory )
# add_test( NAME betr_test_event_trigger COMMAND $<TARGET_FILE:betr_test_all> test_event_trigger )
# add_test( NAME betr_test_edgel_change_detection COMMAND $<TARGET_FILE:betr_test_all> test_edgel_change_detection )
# add_test( NAME betr_test_json_params COMMAND $<TARGET_FILE:betr_test_all> test_json_params )
