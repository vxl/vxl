# segmentation detection library
# brl/sdet/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
SET(sdet_sources
    sdet_edge_champher.cxx     sdet_edge_champher.h
    sdet_contour.cxx           sdet_contour.h
    sdet_detector_params.cxx   sdet_detector_params.h
    sdet_detector.cxx          sdet_detector.h
)
AUX_SOURCE_DIRECTORY(Templates sdet_sources)

ADD_LIBRARY(sdet ${sdet_sources})

TARGET_LINK_LIBRARIES(sdet btol bdgl gevd vdgl vtol vsol vil vnl vbl)

