#
# ./v3p/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


SUBDIRS(netlib)

IF(JPEG)
  # do nothing
ELSE(JPEG)
 SUBDIRS(jpeg)
ENDIF(JPEG)

