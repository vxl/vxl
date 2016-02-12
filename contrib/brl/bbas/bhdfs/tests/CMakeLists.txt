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

add_executable( bhdfs_test_all
  test_driver.cxx
  test_hadoop.cxx  # start hadoop and run this test to see if hadoop and vxl are hookep fine.
  test_manager.cxx
  test_fstream.cxx
  test_vil_stream.cxx
  test_vil_save_load_image.cxx
)

target_link_libraries( bhdfs_test_all bhdfs ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib )

add_test( NAME bhdfs_test_manager COMMAND $<TARGET_FILE:bhdfs_test_all> test_manager )
add_test( NAME bhdfs_test_manager COMMAND $<TARGET_FILE:bhdfs_test_all> test_fstream )
add_test( NAME bhdfs_test_manager COMMAND $<TARGET_FILE:bhdfs_test_all> test_vil_stream )
add_test( NAME bhdfs_test_manager COMMAND $<TARGET_FILE:bhdfs_test_all> test_vil_save_load_image )
add_test( NAME bhdfs_test_manager COMMAND $<TARGET_FILE:bhdfs_test_all> test_hadoop )

add_executable( bhdfs_test_include test_include.cxx )
target_link_libraries( bhdfs_test_include bhdfs )
