#include "bvxm_process_utils.h"
#include <vcl_iostream.h>

#include <vgl/vgl_point_3d.h>

bool bvxm_process_utils::generate_test_boxes(double box_min_x, double box_min_y, double box_min_z, 
                    double box_dim_x, double box_dim_y, double box_dim_z,
                    double world_dim_x, double world_dim_y, double world_dim_z,
                    vcl_vector<vgl_box_3d<double> >& boxes)
{
  // create the big box at the bottom
  double max_x = box_min_x + box_dim_x;
  double max_y = box_min_y + box_dim_y;
  double max_z = box_min_z + box_dim_z;
  if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) {
    vcl_cerr << "generate_boxes() -- the box is out of world boundaries!" << vcl_endl;
    return false;
  }

  vgl_box_3d<double> box(box_min_x, box_min_y, box_min_z, max_x, max_y, max_z);
  boxes.push_back(box);

  // create the top boxe
  vgl_point_3d<double> centroid = box.centroid();
  // make the top box 2/3 of the size of the previous one 
  double dimx = 2*(box.max_x() - box.min_x())/3;
  double dimy = 2*(box.max_y() - box.min_y())/3;
  double dimz = 2*(box.max_z() - box.min_z())/3;
  centroid.set(centroid.x(), centroid.y(), box.max_z() + dimz/2.0);
  vgl_box_3d<double> top_box = vgl_box_3d<double> (centroid, dimx, dimy, dimz, vgl_box_3d<double>::centre);
  // translate it a bit
  vgl_point_3d<double> top_centroid = top_box.centroid();
  top_box.set_centroid(vgl_point_3d<double>(top_centroid.x()+dimx/3., top_centroid.y()+dimx/3., top_centroid.z()));
  // check if the box in the world completely
  max_x = top_box.max_x();
  max_y = top_box.max_y();
  max_z = top_box.max_z();
  // stop if the new box is getting out of the boundaries
  if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) 
    return false;
  boxes.push_back(top_box);
  
  return true;
}

vcl_vector<vgl_point_3d<double> >
bvxm_process_utils::corners_of_box_3d(vgl_box_3d<double> box)
{
  vcl_vector<vgl_point_3d<double> > corners;

  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y(), box.max_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.max_z()));
  corners.push_back(box.max_point());
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.max_z()));
  return corners;
}
