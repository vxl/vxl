# segmentation image processing library
# bseg/brip/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
SET(brip_sources
   brip_float_ops.cxx    brip_float_ops.h
   brip_roi.cxx    brip_roi.h
)
AUX_SOURCE_DIRECTORY(Templates brip_sources)

ADD_LIBRARY(brip ${brip_sources})

TARGET_LINK_LIBRARIES(brip vil1 vnl vbl vul)
