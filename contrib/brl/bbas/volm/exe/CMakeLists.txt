# contrib/brl/bbas/volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/ )

ADD_EXECUTABLE( volm_dummy_matcher
                volm_dummy_matcher.cxx
              )

ADD_EXECUTABLE( volm_query_creation
                volm_query_creation.cxx
              )

ADD_EXECUTABLE( volm_generate_hypos
                volm_generate_hypos.cxx
              )

ADD_EXECUTABLE( volm_evaluate_output_tiles
                volm_evaluate_output_tiles.cxx
              )

ADD_EXECUTABLE( volm_construct_cam_space_query 
                volm_construct_cam_space_query.cxx
              )

ADD_EXECUTABLE( volm_evaluate
                volm_evaluate.cxx
              )

ADD_EXECUTABLE( volm_land_class
                volm_land_classification_tools.cxx
              )

ADD_EXECUTABLE( volm_land_class_lidar
                volm_land_classify_lidar.cxx
              )

ADD_EXECUTABLE( volm_prepare_landclass_images
                volm_prepare_landclass_images.cxx
              )
              
ADD_EXECUTABLE( volm_create_osm_objects
                volm_create_osm_objects.cxx
              )

ADD_EXECUTABLE( volm_generate_osm
                volm_generate_osm.cxx
              )

ADD_EXECUTABLE( volm_create_bvxm_scenes
                volm_create_bvxm_scenes.cxx
              )

ADD_EXECUTABLE( volm_sort_bvxm_scene_urban
                volm_sort_bvxm_scene_urban.cxx
              )
              
ADD_EXECUTABLE( volm_create_osm_2d_map
                volm_create_osm_2d_map.cxx
              )
              
ADD_EXECUTABLE( volm_refine_height_map
                volm_refine_height_map.cxx
              )
              
ADD_EXECUTABLE( volm_create_osm_intersection_kml
                volm_create_osm_intersection_kml.cxx
              )
          
ADD_EXECUTABLE( volm_create_osm_kml
                volm_create_osm_kml.cxx
              )
              
TARGET_LINK_LIBRARIES( volm_dummy_matcher volm depth_map )
TARGET_LINK_LIBRARIES( volm_query_creation volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_generate_hypos volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_evaluate_output_tiles volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_construct_cam_space_query volm vbl vgl depth_map bvrml)
TARGET_LINK_LIBRARIES( volm_evaluate volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_land_class volm depth_map sdet bvrml bkml )
TARGET_LINK_LIBRARIES( volm_land_class_lidar volm depth_map sdet bvrml bkml )
TARGET_LINK_LIBRARIES( volm_prepare_landclass_images volm depth_map sdet bvrml bkml )
TARGET_LINK_LIBRARIES( volm_create_osm_objects volm vbl vgl bkml )
TARGET_LINK_LIBRARIES( volm_generate_osm volm vul vgl )
TARGET_LINK_LIBRARIES( volm_create_osm_2d_map vul vgl volm )
TARGET_LINK_LIBRARIES( volm_create_bvxm_scenes volm vbl vgl bkml vpgl bvxm )
TARGET_LINK_LIBRARIES( volm_sort_bvxm_scene_urban volm vul vpgl )
TARGET_LINK_LIBRARIES( volm_refine_height_map volm vil vcl vul vgl )
TARGET_LINK_LIBRARIES( volm_create_osm_intersection_kml volm vil vcl vul vgl bkml)
TARGET_LINK_LIBRARIES( volm_create_osm_kml volm vil vcl vul vgl bkml)