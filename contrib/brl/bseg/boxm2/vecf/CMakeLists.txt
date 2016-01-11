# library to support vector field operations

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${RPL_INCLUDE_DIR} )
message("RPL_INCLUDE_DIR = " ${RPL_INCLUDE_DIR})
SET(boxm2_vecf_sources
    boxm2_vecf_eye_params.h
    boxm2_vecf_eye_scene.h                  boxm2_vecf_eye_scene.cxx
    boxm2_vecf_head_model.h                 boxm2_vecf_head_model.cxx
    boxm2_vecf_composite_head_parameters.h
    boxm2_vecf_composite_head_model.h       boxm2_vecf_composite_head_model.cxx
    boxm2_vecf_articulated_scene.h          boxm2_vecf_articulated_scene.cxx
    boxm2_vecf_articulated_params.h
    boxm2_vecf_orbit_scene.h                boxm2_vecf_orbit_scene.cxx
    boxm2_vecf_lid_base.h                   boxm2_vecf_lid_base.cxx
    boxm2_vecf_eyelid.h                     boxm2_vecf_eyelid.cxx
    boxm2_vecf_scene_articulation.h
    boxm2_vecf_orbit_articulation.h         boxm2_vecf_orbit_articulation.cxx
    boxm2_vecf_composite_head_model_articulation.h         boxm2_vecf_composite_head_model_articulation.cxx
    boxm2_vecf_eyelid_crease.h              boxm2_vecf_eyelid_crease.cxx
    boxm2_vecf_fit_orbit.h                  boxm2_vecf_fit_orbit.cxx
    boxm2_vecf_orbit_params.h               boxm2_vecf_orbit_params.cxx
    boxm2_vecf_fit_margins.h                boxm2_vecf_fit_margins.cxx
    boxm2_vecf_appearance_extractor.h       boxm2_vecf_appearance_extractor.cxx
    boxm2_vecf_orbit_param_stats.h          boxm2_vecf_orbit_param_stats.cxx
    boxm2_vecf_point_cloud_orbit_viewer.h   boxm2_vecf_point_cloud_orbit_viewer.cxx
    boxm2_vecf_plot_orbit.h                 boxm2_vecf_plot_orbit.cxx
    boxm2_vecf_orbit_exporter.h             boxm2_vecf_orbit_exporter.cxx
    boxm2_vecf_vector_field_base.h          boxm2_vecf_vector_field_base.cxx
    boxm2_vecf_vector_field.h               boxm2_vecf_vector_field.txx
    boxm2_vecf_similarity_transform.h       boxm2_vecf_similarity_transform.cxx
    boxm2_vecf_inverse_square_weighting_function.h        boxm2_vecf_inverse_square_weighting_function.cxx
    boxm2_vecf_landmark_warp.h              boxm2_vecf_landmark_warp.txx
    boxm2_vecf_composite_transform.h        boxm2_vecf_composite_transform.cxx
    boxm2_vecf_mandible.h                   boxm2_vecf_mandible.cxx
    boxm2_vecf_spline_field.h               boxm2_vecf_spline_field.cxx
    boxm2_vecf_labeled_point.h              boxm2_vecf_labeled_point.cxx
    boxm2_vecf_fit_skull.h                  boxm2_vecf_fit_skull.cxx
    boxm2_vecf_mandible_params.h            boxm2_vecf_mandible_params.cxx
    boxm2_vecf_mandible_scene.h             boxm2_vecf_mandible_scene.cxx
    boxm2_vecf_mandible_articulation.h      boxm2_vecf_mandible_articulation.cxx
    boxm2_vecf_cranium.h                    boxm2_vecf_cranium.cxx
    boxm2_vecf_cranium_params.h             boxm2_vecf_cranium_params.cxx
    boxm2_vecf_cranium_scene.h              boxm2_vecf_cranium_scene.cxx
    boxm2_vecf_cranium_articulation.h       boxm2_vecf_cranium_articulation.cxx
    boxm2_vecf_skull_params.h               boxm2_vecf_skull_params.cxx
    boxm2_vecf_skull_scene.h                boxm2_vecf_skull_scene.cxx
    boxm2_vecf_skull_articulation.h         boxm2_vecf_skull_articulation.cxx
)

AUX_SOURCE_DIRECTORY(Templates boxm2_vecf_sources)

ADD_LIBRARY(boxm2_vecf  ${boxm2_vecf_sources})
TARGET_LINK_LIBRARIES(boxm2_vecf boxm2_io boxm2_cpp_algo brip baio bvrml vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map rsdl)
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  TARGET_LINK_LIBRARIES(boxm2_vecf boxm2_ocl boxm2_ocl_algo  bocl )
ENDIF()
# OpenCL support code
add_subdirectory(ocl)


#processes
#subdirs(pro)

# viewing for articulated scenes
subdirs(view)

# fitting executables
subdirs(exe)

#tests
IF( BUILD_TESTING )
  add_subdirectory(tests)
ENDIF( BUILD_TESTING )
