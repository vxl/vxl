INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

ADD_EXECUTABLE( vul_test_all     vul_test_all.cxx )
ADD_EXECUTABLE( vul_test_arg     vul_test_arg.cxx )
ADD_EXECUTABLE( vul_test_sprintf vul_test_sprintf.cxx )
ADD_EXECUTABLE( vul_test_root_dir vul_test_root_dir.cxx )
