#ifndef bwm_lidar_algo_t_
#define bwm_lidar_algo_t_

#include <vcl_vector.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

class lidar_labeling_params 
{
public:
  lidar_labeling_params(double gnd_thresh = 1.0, double bld_diff_thresh = 0.5, 
    double min_bld_height = 2.5, double min_bld_area = 35,
    double max_veg_height = 10.0, double veg_diff_thresh = 0.75)
    : gnd_thresh_(gnd_thresh), bld_diff_thresh_(bld_diff_thresh), 
    min_bld_height_(min_bld_height), min_bld_area_(min_bld_area),
    max_veg_height_(max_veg_height), veg_diff_thresh_(veg_diff_thresh) {}

  double gnd_thresh_;         // maximum ground height
  double bld_diff_thresh_;    // maximum first/last return diff for building
  double min_bld_height_;     // minimum building height
  double min_bld_area_;       // minimum building area (in pixels)
  double max_veg_height_;     // anything above will automatically be labeled building
  double veg_diff_thresh_;
};


class bwm_lidar_algo {
public:
  static void label_lidar(vil_image_view<float> first_ret, 
    vil_image_view<float> last_ret, 
    lidar_labeling_params& params,
    vil_image_view<vxl_byte> &labeled);
};

#endif

