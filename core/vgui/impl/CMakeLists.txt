#
# ./oxl/vgui/impl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

IF(WIN32)
  SUBDIRS(mfc)
ELSE(WIN32)
  SUBDIRS(gtk)
ENDIF(WIN32)

