# video processor library
# brl/bseg/vpro/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(vpro_sources
    vpro_video_process.cxx           vpro_video_process.h  vpro_video_process_sptr.h
    vpro_region_process.cxx          vpro_region_process.h
    vpro_capture_process.cxx         vpro_capture_process.h
    vpro_frame_diff_params.cxx       vpro_frame_diff_params.h
    vpro_frame_diff_process.cxx      vpro_frame_diff_process.h
    vpro_motion_params.cxx           vpro_motion_params.h
    vpro_motion_process.cxx          vpro_motion_process.h
    vpro_lucas_kanade_process.cxx    vpro_lucas_kanade_process.h
    vpro_epipolar_space_process.cxx  vpro_epipolar_space_process.h
    vpro_edge_process.cxx            vpro_edge_process.h
    vpro_harris_corner_process.cxx   vpro_harris_corner_process.h
    vpro_line_fit_process.cxx        vpro_line_fit_process.h
    vpro_edge_line_process.cxx       vpro_edge_line_process.h
    vpro_grid_finder_process.cxx     vpro_grid_finder_process.h
    vpro_basis_generator_process.cxx vpro_basis_generator_process.h
    vpro_fourier_params.cxx          vpro_fourier_params.h
    vpro_fourier_process.cxx         vpro_fourier_process.h
    vpro_spatial_filter_params.cxx   vpro_spatial_filter_params.h
    vpro_spatial_filter_process.cxx  vpro_spatial_filter_process.h
    vpro_ihs_process.cxx             vpro_ihs_process.h
)
AUX_SOURCE_DIRECTORY(Templates vpro_sources)

ADD_LIBRARY(vpro ${vpro_sources})

TARGET_LINK_LIBRARIES(vpro sdet brip gevd vtol vsol vidl_vil1 vil1 vul vbl)
