# photogrammetry
SUBDIRS(vpgl)

# CARMEN
SUBDIRS(carmen_interface)

# tests
IF(BUILD_TESTS)
  SUBDIRS(vpgl_tests)
ENDIF(BUILD_TESTS)
