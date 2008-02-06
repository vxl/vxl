# brl/bpro/bipp/io/CMakeLists.txt

## These two files should probably be in vil/io, leaving them here for now until we are sure.

SET(bipp_io_sources
    vil_io_image_view_base.h vil_io_image_view_base.cxx
)

ADD_LIBRARY(bipp_io ${bipp_io_sources})
TARGET_LINK_LIBRARIES(bipp_io vsl vil vil_io)


