# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "22:00:00 EST")

#
# Client configuration
#
SET (DROP_SITE "ftp.cs.rpi.edu")
SET (DROP_LOCATION "/private/vision/incoming")
SET (DROP_SITE_USER "ftp")
SET (DROP_SITE_PASSWORD "vxl-tester@somewhere.com")
SET (TRIGGER_SITE "http://cgi.cs.rpi.edu/~perera/vxl/submit-testing-results.cgi")

SET (CVS_WEB_URL "http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/vxl/")

INCLUDE( ${allvxl_SOURCE_DIR}/DartConfigServer.cmake OPTIONAL )
