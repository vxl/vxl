# CTest Configuration for VXL

set( CTEST_PROJECT_NAME "VXL" )
set( CTEST_NIGHTLY_START_TIME "22:00:00 EST" )
set( CTEST_DROP_METHOD "ftp" )
set( CTEST_DROP_SITE "ftp.cs.rpi.edu" )
set( CTEST_DROP_LOCATION "/private/vision/incoming" )
set( CTEST_DROP_SITE_USER "ftp" )
set( CTEST_DROP_SITE_PASSWORD "vxl-tester@somewhere.com" )
set( CTEST_DROP_SITE_CDASH FALSE )
set( CTEST_TRIGGER_SITE "http://cgi2.cs.rpi.edu/~perera/vxl/submit-testing-results.cgi" )

set( DETECT_REPEATED_ERRORS ON )
