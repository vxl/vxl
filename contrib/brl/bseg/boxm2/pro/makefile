USES += contrib/brl contrib/brl/bseg core

LIBRARY := boxm2_pro

SOURCES += boxm2_register.cxx

MANPAGE_SOURCES += boxm2_processes.h
SOURCES += processes/boxm2_load_scene_process.cxx
SOURCES += processes/boxm2_create_cache_process.cxx
SOURCES += processes/boxm2_add_block_process.cxx
SOURCES += processes/boxm2_write_scene_xml_process.cxx
SOURCES += processes/boxm2_create_scene_process.cxx
SOURCES += processes/boxm2_export_textured_mesh_process.cxx
SOURCES += processes/boxm2_texture_mesh_process.cxx
SOURCES += processes/boxm2_export_mesh_process.cxx
SOURCES += processes/boxm2_describe_scene_process.cxx
SOURCES += processes/boxm2_create_stream_cache_process.cxx
SOURCES += processes/boxm2_scene_illumination_info_process.cxx
SOURCES += processes/boxm2_roi_init_process.cxx
SOURCES += processes/boxm2_persp_cam_from_scene_process.cxx
SOURCES += processes/boxm2_write_cache_process.cxx
SOURCES += processes/boxm2_mask_sift_features_process.cxx
SOURCES += processes/boxm2_compute_sun_affine_camera_process.cxx
SOURCES += processes/boxm2_bundle_to_scene_process.cxx
SOURCES += processes/boxm2_blob_change_detection_process.cxx
SOURCES += processes/boxm2_clear_cache_process.cxx
SOURCES += processes/boxm2_blob_precision_recall_process.cxx
SOURCES += processes/boxm2_export_visibility_wrl_process.cxx
SOURCES += processes/boxm2_export_point_cloud_xyz_process.cxx
SOURCES += processes/boxm2_scene_bbox_process.cxx
SOURCES += processes/boxm2_transform_model_process.cxx
SOURCES += processes/boxm2_paint_mesh_process.cxx
SOURCES += processes/boxm2_create_scene_mask_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
