# brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
SUBDIRS(brip)

# segmentation feature detection library
SUBDIRS(sdet)

# video processors
SUBDIRS(vpro)

# tracking
SUBDIRS(strk)

# misc 
#SUBDIRS(bcons)

# segmentation view library
SUBDIRS(segv)

# segmentation application executables
SUBDIRS(sbin)

# segmentation tests
IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
