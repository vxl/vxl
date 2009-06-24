# contrib/brl/bseg/bvpl/io/CMakeLists.txt

SET(bvpl_io_sources
    bvpl_io_kernel.h       bvpl_io_kernel.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvpl_io_sources)

ADD_LIBRARY(bvpl_io ${bvpl_io_sources})
TARGET_LINK_LIBRARIES(bvpl_io bvpl vsl vbl)


