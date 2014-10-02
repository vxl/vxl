#This is contrib/brl/bseg/boxm2/volm/conf/tests/CMakeLists.txt
ADD_EXECUTABLE( boxm2_volm_conf_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_conf_test_include boxm2_volm_conf )