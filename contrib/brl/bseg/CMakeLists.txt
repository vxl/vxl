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

# markov random fields
SUBDIRS(bmrf)

# misc
#SUBDIRS(bcons)

# segmentation view library
SUBDIRS(segv)

# segmentation application executables
SUBDIRS(sbin)
