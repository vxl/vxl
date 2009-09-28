# contrib/brl/bseg/bvplutil//io/CMakeLists.txt

SET(bvpl_util_io_sources
    bvpl_corner_pairs_io.h       bvpl_corner_pairs_io.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvpl_util_io_sources)

ADD_LIBRARY(bvpl_util_io ${bvpl_util_io_sources})
TARGET_LINK_LIBRARIES(bvpl_util_io bvpl_util vsl vbl)


