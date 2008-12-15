# photogrammetry
SUBDIRS(vpgl)

# CARMEN
SUBDIRS(carmen_interface)

IF( BUILD_TESTING )
  SUBDIRS(vpgl_tests)
ENDIF( BUILD_TESTING )
