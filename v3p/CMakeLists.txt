

SUBDIRS(netlib)

IF(JPEG)
  # do nothing
ELSE(JPEG)
 SUBDIRS(jpeg)
ENDIF(JPEG)

