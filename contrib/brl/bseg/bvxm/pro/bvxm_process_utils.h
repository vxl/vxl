#ifndef bvxm_process_utils_h_
#define bvxm_process_utils_h_

#include <vcl_vector.h>
#include <vgl/vgl_box_3d.h>

class bvxm_process_utils
{
public:
  static bool generate_test_boxes(double box_min_x, double box_min_y, double box_min_z, 
                    double box_dim_x, double box_dim_y, double box_dim_z,
                    double world_dim_x, double world_dim_y, double world_dim_z,
                    vcl_vector<vgl_box_3d<double> >& boxes);

  static vcl_vector<vgl_point_3d<double> > corners_of_box_3d(vgl_box_3d<double> box);
};

#endif