# gel/vgel/kl/CMakeLists.txt

SOURCE_FILES(vgel_kl_sources
                      base.h
 convolve.c           convolve.h
 klt.c                klt.h
 pnmio.c              pnmio.h
 selectGoodFeatures.c
 trackFeatures.c
 error.c              error.h
 klt_util.c           klt_util.h
 pyramid.c            pyramid.h
 storeFeatures.c
 writeFeatures.c
)

ADD_LIBRARY(vgel_kl vgel_kl_sources)
