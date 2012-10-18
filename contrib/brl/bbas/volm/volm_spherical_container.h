//This is brl/bbas/volm/volm_spherical_container.h
#ifndef volm_spherical_container_h_
#define volm_spherical_container_h_
//:
// \file
// \brief  A class to represent a spherical containor with a fixed voxelization
//
// \author Yi Dong
// \date October 07, 2012
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cmath.h>
#include <vcl_map.h>
#include <vgl/vgl_point_3d.h>
#include <bvrml/bvrml_write.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>


class volm_voxel : public vbl_ref_count
{
 public:
  enum ATTRIBUTES { SKY, PLANAR_HORIZONTAL, PLANAR_VERTICAL, PLANAR_OTHER, NON_PLANAR };

  volm_voxel(double resolution, vgl_point_3d<double> cent)
  : resolution_(resolution),center_(cent) {}

  double resolution_;
  vgl_point_3d<double>  center_;
};

class volm_spherical_container : public vbl_ref_count
{
 public:
  // constructor
  volm_spherical_container() {}
  volm_spherical_container(double d_solid_ang, double voxel_min, double max_dist);

  // accessor
  vcl_vector<volm_voxel>& get_voxels() { return voxels_; }
  vcl_map<double, unsigned int>& get_depth_offset_map() { return depth_offset_map_;}

  void draw_template(vcl_string vrml_file_name, double dmin);

  double min_voxel_res() { return vmin_; }

  //: return the offset and depth of the last layer with vmin resolution
  void last_vmin(unsigned int& offset, double& depth);

 protected:
  bool meshcurrentlayer(double d, double vc);
  vcl_vector<volm_voxel> voxels_;
  vcl_map<double, unsigned int> depth_offset_map_;                        // stores offset for each layer at depth d along east direction
  vcl_map<double, vnl_vector_fixed<unsigned int,5> > depth_blk_num_map_;  // stores offset of each facet voxels of the cube at depth d along east direction
  unsigned int depth_offset_;
  double ds_;
  double vmin_;
  double dmax_;
};

#endif  // volm_spherical_container_h_

