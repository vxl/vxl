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

#
# Server configuration
#
SET (CVS_WEB_URL "http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/vxl/vxl/")
SET (USE_GNATS "ON")
SET (GNATS_WEB_URL "http://sourceforge.net/tracker/?group_id=24293")

#  (not TRUE, not YES, just ON)
SET (DELIVER_CONTINUOUS_EMAIL "ON")

SET (CONTINUOUS_FROM "vxl-maintainers@lists.sourceforge.net")
# This is probably best set on the server build's cache
# SET (SMTP_MAILHOST "a smtp server")
SET (DEFAULT_CONTINUOUS_DOMAIN "users.sourceforge.net")
SET (CONTINUOUS_MONITOR_LIST "")
SET (CONTINUOUS_BASE_URL "http://www.cs.rpi.edu/research/vision/vxl")
SET (PROJECT_NAME "VXL")

