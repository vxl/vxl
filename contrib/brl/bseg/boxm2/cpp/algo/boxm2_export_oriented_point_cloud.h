#ifndef boxm2_export_oriented_point_cloud_h_included_
#define boxm2_export_oriented_point_cloud_h_included_
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

class boxm2_export_oriented_point_cloud
{
public:
  static bool export_oriented_point_cloud(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache,
                                          const std::string& output_filename, bool output_aux = false,
                                          float vis_t=0.0f, float nmag_t=0.0f,
                                          float prob_t=0.0f, float exp_t=0.0f,
                                          const std::string& bb_filename="");

};

#endif
