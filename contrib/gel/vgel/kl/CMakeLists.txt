# ./gel/vgel/kl/CMakeLists.txt

ADD_LIBRARY(vgel_kl vgel_kl_sources)

SOURCE_FILES(vgel_kl_sources
  convolve.c
  klt.c
  pnmio.c
  selectGoodFeatures.c
  trackFeatures.c
  error.c
  klt_util.c
  pyramid.c
  storeFeatures.c
  writeFeatures.c
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vgel/kl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
