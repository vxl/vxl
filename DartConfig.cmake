# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "22:00:00 EST" CACHE STRING "Agreed CVS snapshot timestamp for nightly builds.")

#
# Client configuration
#

SET (DROP_SITE "ftp.cs.rpi.edu" CACHE STRING "Address of remote drop site.")
SET (DROP_LOCATION "/private/vision/incoming" CACHE STRING "Directory of drop site.")
SET (DROP_SITE_USER "ftp" CACHE STRING "Username for remote access to drop site.")
SET (DROP_SITE_PASSWORD "vxl-tester@somewhere.com" CACHE STRING "Password for remote access to drop site.")

SET (TRIGGER_SITE "http://cgi.cs.rpi.edu/~perera/vxl/submit-testing-results.cgi" CACHE STRING "URL for drop notification.")

MARK_AS_ADVANCED(DROP_SITE DROP_LOCATION DROP_SITE_USER DROP_SITE_PASSWORD TRIGGER_SITE NIGHTLY_START_TIME)

INCLUDE( ${allvxl_SOURCE_DIR}/DartConfigServer.cmake OPTIONAL )
