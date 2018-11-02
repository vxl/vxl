#ifndef bmdl_defs_h_
#define bmdl_defs_h_

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

bool generate_mesh(std::string fpath_poly, vil_image_view_base_sptr label_img,
                   vil_image_view_base_sptr height_img, vil_image_view_base_sptr ground_img,
                   std::string fpath_mesh, vpgl_geo_camera* const lidar_cam, unsigned& num_of_buildings);


bool trace_boundaries(vil_image_view_base_sptr label_img, std::string fpath);

bool classify(vil_image_view_base_sptr lidar_first,
              vil_image_view_base_sptr lidar_last,
              vil_image_view_base_sptr ground,
              vil_image_view_base_sptr& label_img,
              vil_image_view_base_sptr& height_img,
              float gthresh, float vthresh, float athresh, float hres);

bool lidar_roi(unsigned type,  //0 for geo coordinates, 1 for image coord
               vil_image_resource_sptr lidar_first,
               vil_image_resource_sptr lidar_last,
               vil_image_resource_sptr ground,
               float min_lat, float min_lon,
               float max_lat, float max_lon,
               vil_image_view_base_sptr& first_roi,
               vil_image_view_base_sptr& last_roi,
               vil_image_view_base_sptr& ground_roi,
               vpgl_geo_camera*& camera);

#endif // bmdl_defs_h_
