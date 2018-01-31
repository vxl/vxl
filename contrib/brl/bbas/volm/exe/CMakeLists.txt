# contrib/brl/bbas/volm/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/ )

add_executable( volm_dummy_matcher
                volm_dummy_matcher.cxx
              )

add_executable( volm_query_creation
                volm_query_creation.cxx
              )

add_executable( volm_generate_hypos
                volm_generate_hypos.cxx
              )

add_executable( volm_evaluate_output_tiles
                volm_evaluate_output_tiles.cxx
              )

add_executable( volm_construct_cam_space_query
                volm_construct_cam_space_query.cxx
              )

add_executable( volm_evaluate
                volm_evaluate.cxx
              )

add_executable( volm_land_class
                volm_land_classification_tools.cxx
              )

add_executable( volm_land_class_lidar
                volm_land_classify_lidar.cxx
              )

add_executable( volm_prepare_landclass_images
                volm_prepare_landclass_images.cxx
              )

add_executable( volm_create_osm_objects
                volm_create_osm_objects.cxx
              )

add_executable( volm_generate_osm
                volm_generate_osm.cxx
              )

add_executable( volm_create_bvxm_scenes
                volm_create_bvxm_scenes.cxx
              )

add_executable( volm_sort_bvxm_scene_urban
                volm_sort_bvxm_scene_urban.cxx
              )

add_executable( volm_create_osm_2d_map
                volm_create_osm_2d_map.cxx
              )

add_executable( volm_create_osm_2d_mask_for_geotiff
                volm_create_osm_2d_mask_for_geotiff.cxx
              )

add_executable( volm_refine_height_map
                volm_refine_height_map.cxx
              )

add_executable( volm_create_osm_intersection_kml
                volm_create_osm_intersection_kml.cxx
              )

add_executable( volm_create_osm_kml
                volm_create_osm_kml.cxx
              )

add_executable( volm_parse_candidate_region
                volm_parse_candidate_region.cxx
              )

target_link_libraries( volm_dummy_matcher volm depth_map )
target_link_libraries( volm_query_creation volm depth_map bvrml )
target_link_libraries( volm_generate_hypos volm depth_map bvrml )
target_link_libraries( volm_evaluate_output_tiles volm depth_map bvrml )
target_link_libraries( volm_construct_cam_space_query volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl depth_map bvrml)
target_link_libraries( volm_evaluate volm depth_map bvrml )
target_link_libraries( volm_land_class volm depth_map sdet bvrml bkml )
target_link_libraries( volm_land_class_lidar volm depth_map sdet bvrml bkml )
target_link_libraries( volm_prepare_landclass_images volm depth_map sdet bvrml bkml )
target_link_libraries( volm_create_osm_objects volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl bkml )
target_link_libraries( volm_generate_osm volm ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl )
target_link_libraries( volm_create_osm_2d_map ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl volm )
target_link_libraries( volm_create_osm_2d_mask_for_geotiff ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl volm )
target_link_libraries( volm_create_bvxm_scenes volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl bkml ${VXL_LIB_PREFIX}vpgl bvxm )
target_link_libraries( volm_sort_bvxm_scene_urban volm ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpgl )
target_link_libraries( volm_refine_height_map volm ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl )
target_link_libraries( volm_create_osm_intersection_kml volm ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl bkml )
target_link_libraries( volm_create_osm_kml volm ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgl bkml )
target_link_libraries( volm_parse_candidate_region volm ${VXL_LIB_PREFIX}vgl bkml )
