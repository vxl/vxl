# library to support vector field operations

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${RPL_INCLUDE_DIR} )

set(boxm2_vecf_sources
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
    boxm2_vecf_estimate_camera_from_canthus.h boxm2_vecf_estimate_camera_from_canthus.cxx
    boxm2_vecf_appearance_extractor.h       boxm2_vecf_appearance_extractor.cxx
    boxm2_vecf_orbit_param_stats.h          boxm2_vecf_orbit_param_stats.cxx
    boxm2_vecf_point_cloud_orbit_viewer.h   boxm2_vecf_point_cloud_orbit_viewer.cxx
    boxm2_vecf_plot_orbit.h                 boxm2_vecf_plot_orbit.cxx
    boxm2_vecf_orbit_exporter.h             boxm2_vecf_orbit_exporter.cxx
    boxm2_vecf_vector_field_base.h          boxm2_vecf_vector_field_base.cxx
    boxm2_vecf_vector_field.h               boxm2_vecf_vector_field.hxx
    boxm2_vecf_similarity_transform.h       boxm2_vecf_similarity_transform.cxx
    boxm2_vecf_inverse_square_weighting_function.h        boxm2_vecf_inverse_square_weighting_function.cxx
    boxm2_vecf_landmark_warp.h              boxm2_vecf_landmark_warp.hxx
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
    boxm2_vecf_skin.h                       boxm2_vecf_skin.cxx
    boxm2_vecf_skin_params.h                boxm2_vecf_skin_params.cxx
    boxm2_vecf_skin_scene.h                 boxm2_vecf_skin_scene.cxx
    boxm2_vecf_composite_face_scene.h       boxm2_vecf_composite_face_scene.cxx
    boxm2_vecf_composite_face_params.h      boxm2_vecf_composite_face_params.cxx
    boxm2_vecf_composite_face_articulation.h    boxm2_vecf_composite_face_articulation.cxx
    boxm2_vecf_mouth_params.h               boxm2_vecf_mouth_params.cxx
    boxm2_vecf_mouth.h                      boxm2_vecf_mouth.cxx
    boxm2_vecf_fit_face.h                   boxm2_vecf_fit_face.cxx
    boxm2_vecf_middle_fat_pocket.h          boxm2_vecf_middle_fat_pocket.cxx
    boxm2_vecf_middle_fat_pocket_params.h   boxm2_vecf_middle_fat_pocket_params.cxx
    boxm2_vecf_middle_fat_pocket_scene.h    boxm2_vecf_middle_fat_pocket_scene.cxx
    boxm2_vecf_middle_fat_pocket_articulation.h   boxm2_vecf_middle_fat_pocket_articulation.cxx
    boxm2_vecf_orbicularis_oris.h           boxm2_vecf_orbicularis_oris.cxx
    boxm2_vecf_orbicularis_oris_params.h    boxm2_vecf_orbicularis_oris_params.cxx
    boxm2_vecf_geometry_base.h
    boxm2_vecf_fit_fat_pocket.h             boxm2_vecf_fit_fat_pocket.cxx
    boxm2_vecf_shuttle_scene.h              boxm2_vecf_shuttle_scene.cxx
)

aux_source_directory(Templates boxm2_vecf_sources)

vxl_add_library(LIBRARY_NAME boxm2_vecf LIBRARY_SOURCES  ${boxm2_vecf_sources})
target_link_libraries(boxm2_vecf boxm2_io boxm2_cpp_algo brip baio bvrml ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl depth_map rsdl)
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  target_link_libraries(boxm2_vecf boxm2_ocl boxm2_ocl_algo  bocl )
endif()
# OpenCL support code
add_subdirectory(ocl)


#processes
#subdirs(pro)

# viewing for articulated scenes
subdirs(view)

# fitting executables
subdirs(exe)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
