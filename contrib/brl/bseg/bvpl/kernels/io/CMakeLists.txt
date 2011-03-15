# contrib/brl/bseg/bvpl/io/CMakeLists.txt

SET(bvpl_kerneles_io_sources
    bvpl_io_kernel.h       bvpl_io_kernel.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvpl_kerneles_io_sources)

ADD_LIBRARY(bvpl_kernels_io ${bvpl_kerneles_io_sources})
TARGET_LINK_LIBRARIES(bvpl_kernels_io bvpl_kernels vbl_io vsl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
