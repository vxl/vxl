# ./brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications

# segmentation feature detection library
SUBDIRS(sdet)

# segmentation view library
SUBDIRS(segv)

# float image operations
SUBDIRS(brip)

# segmentation application executables
SUBDIRS(sbin)


# segmentation tests
SUBDIRS(tests)
