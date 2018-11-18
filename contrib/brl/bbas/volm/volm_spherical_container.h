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

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <map>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  volm_spherical_container() = default;
  volm_spherical_container(float d_solid_ang, float voxel_min, float max_dist);

  // accessor
  std::vector<volm_voxel>& get_voxels() { return voxels_; }
  std::map<double, unsigned int>& get_depth_offset_map() { return depth_offset_map_;}
  std::map<double, unsigned char>& get_depth_interval_map() { return depth_interval_map_; }
  void get_depth_intervals(std::vector<float>& ints);
  //: find the interval of the given depth value, i.e. return interval such that d1 <= value < d2 (caution: interval id is 1 + index in depth_interval_map_)
  unsigned char get_depth_interval(double value);

  void draw_template(const std::string& vrml_file_name, double dmin);
  void draw_helper(std::ofstream& ofs, double dmin);
  //: paint the wireframe of the voxels with the given ids with the given color
  void draw_template_painted(const std::string& vrml_file_name, double dmin, std::vector<unsigned int>& ids, float r, float g, float b, float trans);
  void draw_template_vis_prob(const std::string& vrml_file_name, double dmin, std::vector<char>& ids);

  double min_voxel_res() { return vmin_; }

  //: return the offset and depth of the last layer with given resolution
  void last_res(double res, unsigned int& offset, unsigned int& end_offset, double& depth);

  //: return the offset and depth of the first layer with the given resolution
  void first_res(double res, unsigned int& offset, unsigned int& end_offset, double& depth);

protected:
  bool meshcurrentlayer(double d, double vc);
  std::vector<volm_voxel> voxels_;
  std::map<double, unsigned int> depth_offset_map_;                        // stores offset for each layer at depth d along east direction
  std::map<double, unsigned char> depth_interval_map_;                     // stores a char to designate depth intervals in depth_offset_map_
  std::map<double, vnl_vector_fixed<unsigned int,5> > depth_blk_num_map_;  // stores offset of each facet voxels of the cube at depth d along east direction
  unsigned int depth_offset_;
  float ds_;
  float vmin_;
  float dmax_;
};

#endif  // volm_spherical_container_h_
