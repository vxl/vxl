# ./brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications

# segmentation feature detection library
SUBDIRS(sdet)

# segmentation view library
SUBDIRS(segv)

# segmentation applicationexecutables
SUBDIRS(sbin)

# segmentation tests
SUBDIRS(tests)
