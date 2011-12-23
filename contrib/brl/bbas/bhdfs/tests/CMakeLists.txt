# brl/bbas/bhdfs/tests/CMakeLists.txt
# Tests for bhdfs library
#
# A note to run executables that will use libdhfs, it requires the environment variable CLASSPATH to be set
# for that one must get the classpath from hadoop:
# TMP=$(/usr/local/hadoop/bin/hadoop classpath)
# and then edit /etc/environment to contain the following:
# CLASSPATH=$CLASSPATH:$TMP
#  e.g. echo $CLASSPATH >> /etc/environment
# 
#

ADD_EXECUTABLE( bhdfs_test_all
  test_driver.cxx
  test_hadoop.cxx  # start hadoop and run this test to see if hadoop and vxl are hookep fine. 
  test_manager.cxx
  test_fstream.cxx
  test_vil_stream.cxx
  test_vil_save_load_image.cxx
)

TARGET_LINK_LIBRARIES( bhdfs_test_all bhdfs vil testlib )

ADD_TEST( bhdfs_test_manager ${EXECUTABLE_OUTPUT_PATH}/bhdfs_test_all test_manager )
ADD_TEST( bhdfs_test_manager ${EXECUTABLE_OUTPUT_PATH}/bhdfs_test_all test_fstream )
ADD_TEST( bhdfs_test_manager ${EXECUTABLE_OUTPUT_PATH}/bhdfs_test_all test_vil_stream )
ADD_TEST( bhdfs_test_manager ${EXECUTABLE_OUTPUT_PATH}/bhdfs_test_all test_vil_save_load_image )
ADD_TEST( bhdfs_test_manager ${EXECUTABLE_OUTPUT_PATH}/bhdfs_test_all test_hadoop )

ADD_EXECUTABLE( bhdfs_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bhdfs_test_include bhdfs )
