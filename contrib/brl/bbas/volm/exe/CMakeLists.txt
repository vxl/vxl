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
target_link_libraries( volm_construct_cam_space_query volm vbl vgl depth_map bvrml)
target_link_libraries( volm_evaluate volm depth_map bvrml )
target_link_libraries( volm_land_class volm depth_map sdet bvrml bkml )
target_link_libraries( volm_land_class_lidar volm depth_map sdet bvrml bkml )
target_link_libraries( volm_prepare_landclass_images volm depth_map sdet bvrml bkml )
target_link_libraries( volm_create_osm_objects volm vbl vgl bkml )
target_link_libraries( volm_generate_osm volm vul vgl )
target_link_libraries( volm_create_osm_2d_map vul vgl volm )
target_link_libraries( volm_create_bvxm_scenes volm vbl vgl bkml vpgl bvxm )
target_link_libraries( volm_sort_bvxm_scene_urban volm vul vpgl )
target_link_libraries( volm_refine_height_map volm vil vcl vul vgl )
target_link_libraries( volm_create_osm_intersection_kml volm vil vcl vul vgl bkml )
target_link_libraries( volm_create_osm_kml volm vil vcl vul vgl bkml )
target_link_libraries( volm_parse_candidate_region volm vgl bkml )
