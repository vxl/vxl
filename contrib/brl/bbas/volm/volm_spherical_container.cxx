#include <iostream>
#include <cmath>
#include "volm_spherical_container.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double RoundUp(double x, double unit)
{
  return std::ceil(x/unit)*unit;
}

double RoundUp2Vmin(double x, double vmin)
{
  if (x < 0 || vmin < 0) {
    std::cout << "ERROR: the voxel size has to be positive" << std::endl;
    return vmin;
  }
  if (x < 2*vmin)
    return vmin;
  else {
    auto k = (unsigned int)std::floor(x/vmin);
    unsigned int count = 0;
    while (k != 1) {
      k /= 2;
      count++;
    }
    k = 1<<count;
    return k*vmin;
  }
}

volm_spherical_container::volm_spherical_container(float d_solid_ang, float voxel_min, float max_dist)
  : depth_offset_(0), ds_(d_solid_ang), vmin_(voxel_min)
 {
  dmax_ = (float)RoundUp(max_dist,vmin_);
  double vmin = vmin_;
  double d = 0;
  double dtor = vnl_math::pi_over_180;
  double tan_ds = std::tan(ds_ * dtor);
  //tan_ds = 1; // for debug purpose
  double vc;
  bool is_even = false; // the first layer has 4 voxels and hence half of it is an odd
  while (d <= dmax_) {
  depth_offset_map_[d] = depth_offset_;
    vc = d * tan_ds;
  if (vc >= 2*vmin && is_even) {
    vc = 2*vmin;
    vmin = vc;
  }
  else {
    vc = vmin;
  }
  // use current voxel size to mesh the space
  //this->meshcurrentlayer(d, vc);
  // update and check current layer has even number of voxel along half
  d = d + vc;
  auto N = (unsigned int)(d/vc);
  is_even = !(N%2);
  } // end of while

  //: create the depth_interval_map_  [0,d1) --> 1, [d1,d2) --> 2, ... [dn,dmax) --> n , n can be 253 max
  unsigned int i = 0;
  for (auto & iter : depth_offset_map_) {
    depth_interval_map_[iter.first] = (unsigned char)i;
    i++;
    if (i > 252) {
      std::cerr << "In volm_spherical_container::volm_spherical_container() -- number of depth intervals is larger than 253! Current indexing scheme does not support this container extent of " << dmax_ << "!\n";
      break;
    }
  }
}

//: find the interval of the given depth value, i.e. return interval such that d1 <= value < d2, interval ids are 1 greater than index in depth_offset_map_
unsigned char volm_spherical_container::get_depth_interval(double value)
{
  if (value < 0)
    return (unsigned char)1;
  auto iter = depth_interval_map_.upper_bound(value);
  if (iter == depth_interval_map_.end()) {
    iter--;
    return iter->second + 1;
  }
  return iter->second;
}

bool volm_spherical_container::meshcurrentlayer(double d, double v)
 {
// construct voxel structure at current depth d with current resolution v
  double hv = 0.5*v;
  unsigned int blk_num = 0;
  unsigned int upper_num = 0;
  unsigned int lower_num = 0;
  unsigned int left_num = 0;
  unsigned int right_num = 0;
  unsigned int back_num = 0;
  // construct the upper layer
  std::vector<vgl_point_3d<double> > inners_left;
  std::vector<vgl_point_3d<double> > uppers;
  std::vector<vgl_point_3d<double> > inners_back;
  double z = d + hv;
  double xleft = 0.0;
  double xcr = hv;
  double xcl = -hv;
  double ycr = hv;
  double ycl = -hv;
  double hw = d + v;
  double bdry = d + hv;
  while (xleft < hw) {
    double yleft = 0.0;
    while (yleft < hw) {
      voxels_.emplace_back(v,vgl_point_3d<double>(xcr,ycr,z));
      voxels_.emplace_back(v,vgl_point_3d<double>(xcl,ycr,z));
      voxels_.emplace_back(v,vgl_point_3d<double>(xcl,ycl,z));
      voxels_.emplace_back(v,vgl_point_3d<double>(xcr,ycl,z));
      upper_num += 4;
      blk_num += 4;
      uppers.emplace_back(xcr,ycr,z);
      uppers.emplace_back(xcl,ycr,z);
      uppers.emplace_back(xcl,ycl,z);
      uppers.emplace_back(xcr,ycl,z);
      if (xcr < bdry) {
        inners_left.emplace_back(xcr,ycr,z);
        inners_left.emplace_back(xcl,ycr,z);
        inners_left.emplace_back(xcl,ycl,z);
        inners_left.emplace_back(xcr,ycl,z);
      }
      if (ycr < bdry && xcr < bdry) {
        inners_back.emplace_back(xcr,ycr,z);
        inners_back.emplace_back(xcl,ycr,z);
        inners_back.emplace_back(xcl,ycl,z);
        inners_back.emplace_back(xcr,ycl,z);
      }
      ycr += v; ycl -= v;  yleft += v;
    }
  ycr = hv;  ycl = -hv;
  xcr += v; xcl -= v;  xleft += v;
  }
  // construct lower layer
  for (auto & upper : uppers) {
    voxels_.emplace_back(v,vgl_point_3d<double>(upper.x(),upper.y(),-1*upper.z()));
  lower_num++;
  blk_num++;
  }
  // construct left layer
  vnl_matrix_fixed<double,3,3> Q;
  Q[0][0] = 0.0;  Q[0][1] = 0.0;  Q[0][2] = -1.0;
  Q[1][0] = 0.0;  Q[1][1] = 1.0;  Q[1][2] =  0.0;
  Q[2][0] = 1.0;  Q[2][1] = 0.0;  Q[2][2] =  0.0;
  vnl_vector_fixed<double,3> O;
  O[0] = -d-hv;  O[1] = 0.0;  O[2] = d+hv;
  vnl_vector_fixed<double,3> x;
  vnl_vector_fixed<double,3> X;
  vnl_vector_fixed<double,3> xp;
  vnl_vector_fixed<double,3> Xp;
  std::vector<vgl_point_3d<double> > left;
  for (auto & it : inners_left) {
    X[0] = it.x();  X[1] = it.y();  X[2] = it.z();
  x = Q*(X-O);
  xp[0] = x[2];  xp[1] = x[1];  xp[2] = x[0];
  Xp = Q.transpose()*xp + O;
  voxels_.emplace_back(v, vgl_point_3d<double>(Xp[0],Xp[1],Xp[2]));
  left_num++;
  blk_num++;
  left.emplace_back(Xp[0],Xp[1],Xp[2]);
  }
  // construct right layer
  for (auto & it : left) {
    voxels_.emplace_back(v,vgl_point_3d<double>(-1*it.x(),it.y(),it.z()));
    right_num++;
    blk_num++;
  }
  // construct back layer
  Q[0][0] = 1.0;  Q[0][1] = 0.0;  Q[0][2] =  0.0;
  Q[1][0] = 0.0;  Q[1][1] = 0.0;  Q[1][2] = -1.0;
  Q[2][0] = 0.0;  Q[2][1] = 1.0;  Q[2][2] =  0.0;
  O[0] = 0.0;  O[1] = -d-hv;  O[2] = d+hv;
  std::vector<vgl_point_3d<double> > back;
  for (auto & it : inners_back) {
    X[0] = it.x();  X[1] = it.y();  X[2] = it.z();
    x = Q*(X-O);
    xp[0] = x[0];  xp[1] = x[2];  xp[2] = x[1];
    Xp = Q.transpose()*xp + O;
    voxels_.emplace_back(v, vgl_point_3d<double>(Xp[0],Xp[1],Xp[2]));
    back_num++;
    blk_num++;
    back.emplace_back(Xp[0],Xp[1],Xp[2]);
  }
  // use back layer to mirror front layer
  for (auto & it : back) {
    voxels_.emplace_back(v,vgl_point_3d<double>(it.x(),-1*it.y(),it.z()));
    blk_num++;
  }
  // update current offset
  vnl_vector_fixed<unsigned int,5> surface_index;
  surface_index[0] = upper_num;
  surface_index[1] = lower_num;
  surface_index[2] = left_num;
  surface_index[3] = right_num;
  surface_index[4] = back_num;
  depth_blk_num_map_[d] = surface_index;
  depth_offset_ += blk_num;
  return true;
}

#if 0
double volm_spherical_container::center_depth(volm_voxel voxel)
{
  // given voxel center (x,y,z), return its depth = max(x,y,z)
  double fab_x = std::fabs(voxel.center_.x());
  double fab_y = std::fabs(voxel.center_.y());
  double fab_z = std::fabs(voxel.center_.z());
  double depth = fab_x;
  if (depth < fab_y)
    depth = fab_y;
  if (depth < fab_z)
    depth = fab_z;
  depth -= 0.5*voxel.resolution_;
  return depth;
}

unsigned volm_spherical_container::fetch_voxel(double dist, double theta, double phi)
{
  return 0;
}
#endif

void volm_spherical_container::draw_helper(std::ofstream& ofs, double dmin)
{
  // write a world center and world axis
  double rad = 0.5*vmin_;
  auto it = voxels_.end();
  --it;
  double axis_len = (dmax_ + it->resolution_)*1.2;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(ofs, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_x, (float)axis_len, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_y, (float)axis_len, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_z, (float)axis_len, 1.0f, 1.0f, 1.0f);
  // write the voxel structure
  for (auto & voxel : voxels_) {
    double x = std::fabs(voxel.center_.x());
    double y = std::fabs(voxel.center_.y());
    double z = std::fabs(voxel.center_.z());
    double d = x;
    if (d < y) d = y;
    if (d < z) d = z;
    if (d > dmin) {
      double v_len = voxel.resolution_;
      vgl_point_3d<double> vc(voxel.center_.x(), voxel.center_.y(), voxel.center_.z());
      vgl_box_3d<double> box(vc, v_len, v_len, v_len, vgl_box_3d<double>::centre);
      bvrml_write::write_vrml_box(ofs, box, 1.0f, 1.0f, 0.0f, (float)(d/dmax_+0.2));
      bvrml_write::write_vrml_wireframe_box(ofs, box, 0.0f, 0.0f, 1.0f, 1);
    }
  }
}

void volm_spherical_container::draw_template(const std::string& vrml_file_name, double dmin)
{
  std::ofstream ofs(vrml_file_name.c_str());
  // write the header
  bvrml_write::write_vrml_header(ofs);
  draw_helper(ofs, dmin);
  ofs.close();
}

//: paint the wireframe of the voxels with the given ids with the given color
void volm_spherical_container::draw_template_painted(const std::string& vrml_file_name, double dmin, std::vector<unsigned int>& ids, float r, float g, float b, float trans)
{
  std::ofstream ofs(vrml_file_name.c_str());
  // write the header
  bvrml_write::write_vrml_header(ofs);
  //draw_helper(ofs, dmin);
  // draw the extras

  for (unsigned int id : ids) {

    double x = std::fabs(voxels_[id].center_.x());
    double y = std::fabs(voxels_[id].center_.y());
    double z = std::fabs(voxels_[id].center_.z());
    double d = x;
    if (d < y) d = y;
    if (d < z) d = z;
    if (d > dmin) {
      double v_len = voxels_[id].resolution_;
      vgl_point_3d<double> vc(voxels_[id].center_.x(), voxels_[id].center_.y(), voxels_[id].center_.z());
      vgl_box_3d<double> box(vc, v_len, v_len, v_len, vgl_box_3d<double>::centre);
      bvrml_write::write_vrml_box(ofs, box, r, g, b, trans);
      bvrml_write::write_vrml_wireframe_box(ofs, box, r, g, b, 1);
    }
  }

  ofs.close();
}

//: paint the wireframe of the voxels with the given ids with the given color
void volm_spherical_container::draw_template_vis_prob(const std::string& vrml_file_name, double dmin, std::vector<char>& ids)
{
  std::ofstream ofs(vrml_file_name.c_str());
  // write the header
  bvrml_write::write_vrml_header(ofs);
  //draw_helper(ofs, dmin);
  // draw the extras
  if (voxels_.size() != ids.size()) {
    std::cerr << "In volm_spherical_container::draw_template_vis_prob() -- passed vector is not of the same size with voxels vector!\n";
    return;
  }
  for (unsigned i = 0; i < voxels_.size(); i++) {

    double x = std::fabs(voxels_[i].center_.x());
    double y = std::fabs(voxels_[i].center_.y());
    double z = std::fabs(voxels_[i].center_.z());
    double d = x;
    if (d < y) d = y;
    if (d < z) d = z;
    if (d > dmin) {
      double v_len = voxels_[i].resolution_;
      vgl_point_3d<double> vc(voxels_[i].center_.x(), voxels_[i].center_.y(), voxels_[i].center_.z());
      vgl_box_3d<double> box(vc, v_len, v_len, v_len, vgl_box_3d<double>::centre);
      // VIS_OCC = 0, VIS_UNOCC = 1, NONVIS_UNKNOWN = 2
      if (ids[i] == 0) {
        bvrml_write::write_vrml_box(ofs, box, 0.0f, 0.0f, 0.0f, 0.8f);  // 1 is completely transparent, 0 is completely opaque
        //bvrml_write::write_vrml_wireframe_box(ofs, box, 0.0f, 0.0f, 0.0f, 1.0f); // blacks will be visible and occupied
      } else if (ids[i] == 1) {
        //bvrml_write::write_vrml_box(ofs, box, 0.0f, 0.0f, 1.0f, 0.9f);
        //bvrml_write::write_vrml_wireframe_box(ofs, box, 0.0f, 0.0f, 1.0f, 1.0f); // blues will be visible and unoccupied
      } else if (ids[i] == 2) {
        //bvrml_write::write_vrml_box(ofs, box, 0.0f, 0.0f, 0.0f, 1.0f);
        //bvrml_write::write_vrml_wireframe_box(ofs, box, 0.0f, 1.0f, 0.0f, 0.2f); // greens will be non visible
      }
    }
  }

  ofs.close();
}



//: return the offset and depth of the last layer with given resolution
void volm_spherical_container::last_res(double res, unsigned int& offset, unsigned int& end_offset, double& depth)
{
  // number of depth layers in the container:
  offset = 0;
  depth = 0.0;
  for (auto & iter : depth_offset_map_) {
    if (voxels_[iter.second].resolution_ > res) {
      end_offset = iter.second;
      break;
    }
#ifdef DEBUG
    std::cout << iter->first << ' ' << iter->second << std::endl;
#endif
    offset = iter.second;
    depth = iter.first;
  }
}

//: return the offset and depth of the first layer with the given resolution
void volm_spherical_container::first_res(double res, unsigned int& offset, unsigned int& end_offset, double& depth)
{
  // number of depth layers in the container:
  offset = 0;
  depth = 0.0;
  for (auto iter = depth_offset_map_.begin(); iter != depth_offset_map_.end(); iter++) {
    if (voxels_[iter->second].resolution_ < res)
      continue;
    offset = iter->second;
    depth = iter->first;
    iter++;
    if (iter != depth_offset_map_.end())
      end_offset = iter->second;
    else
      end_offset = (unsigned int)voxels_.size();
    break;
  }
}

void volm_spherical_container::get_depth_intervals(std::vector<float>& ints)
{
  auto iter = depth_interval_map_.begin();
  for (; iter != depth_interval_map_.end(); ++iter)
    ints.push_back((float)iter->first);
}
