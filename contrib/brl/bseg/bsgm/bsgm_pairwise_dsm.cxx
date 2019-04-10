// This is brl/bseg/bsgm/bsgm_pairwise_dsm.cxx

#include "bsgm_pairwise_dsm.h"
#include "bsgm_multiscale_disparity_estimator.h"
#include <limits>
#include <vil/vil_convert.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <bpgl/algo/bpgl_heightmap_from_disparity.h>
#include <bpgl/algo/bpgl_gridding.h>


void bsgm_pairwise_dsm::compute_byte()
{
  vil_convert_stretch_range(rip_.rectified_fview0(), rect_bview0_);
  vil_convert_stretch_range(rip_.rectified_fview1(), rect_bview1_);
  ni_ = rect_bview0_.ni();
  nj_ = rect_bview0_.nj();
}

vil_image_view<vxl_byte> bsgm_pairwise_dsm::invalid_map() const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(invalid_map_, ret);
  return ret;
}

vil_image_view<vxl_byte> bsgm_pairwise_dsm::rev_invalid_map() const
{
  vil_image_view<vxl_byte> ret;
  vil_convert_cast(invalid_map_reverse_, ret);
  return ret;
}

bool bsgm_pairwise_dsm::compute_disparity()
{
  vxl_byte border_val = 0;

  compute_invalid_map(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_, num_disparities(),border_val);

  bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_, num_disparities(), num_active_disparities());

  float invalid_disp = NAN;//required for Dan's implementation of triangulation
  bool good = mde.compute(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_,
                          invalid_disp, params_.multi_scale_mode_, disp_r_);
  return good;
}

bool bsgm_pairwise_dsm::compute_rev_disparity()
{
  vxl_byte border_val = 0;

  compute_invalid_map(rect_bview1_, rect_bview0_, invalid_map_reverse_, min_disparity_, num_disparities(),border_val);

  bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_, num_disparities(), num_active_disparities());

  float invalid_disp = NAN;//required for Dan's implementation of triangulation
  bool good = mde.compute(rect_bview1_, rect_bview0_, invalid_map_reverse_, min_disparity_,
                          invalid_disp, params_.multi_scale_mode_, disp_r_reverse_);
  return good;
}

void bsgm_pairwise_dsm::compute_dsm_and_ptset(vgl_box_3d<double> const& scene_box)
{
  vgl_point_3d<double> pmin = scene_box.min_point(), pmax = scene_box.max_point();
  vgl_point_3d<float> pminf(pmin.x(), pmin.y(), pmin.z()), pmaxf(pmax.x(), pmax.y(), pmax.z());
  vgl_box_3d<float> fbox;
  fbox.add(pminf), fbox.add(pmaxf);
  float samp_dist = params_.point_sample_dist_;
  //
  //                              img0               img1     v
  // in sgm disparity estimation img_targ(x,y) <-> img_ref( x + disp_target(x,y), y )
  // the heightmap code requires  img1(u, v) <--> img0(u - disparity, v)
  // thus the order is reversed                          ^
  //
  height_map_ =  bpgl_heightmap_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, scene_box, samp_dist);

  vgl_point_2d<float> ul(pminf.x(), pmaxf.y());
  std::vector<vgl_point_3d<float> > pts3d;
  ///  bpgl_gridding::pointset_from_grid(height_map_, ul, samp_dist, pts3d);
  bpgl_pointset_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, fbox, samp_dist, pts3d);
  ptset_ = vgl_pointset_3d<float>(pts3d);
}

void bsgm_pairwise_dsm::consistent_heightmap(vgl_box_3d<double> const& scene_box)
{
  size_t n = consist_ptset_.size();
  if(n == 0)
    return;
  std::vector<vgl_point_2d<double> > triangulated_xy;
  std::vector<float> height_values;
  for(size_t i = 0; i<n; ++i){
    const vgl_point_3d<float>& p = consist_ptset_.p(i);
    triangulated_xy.emplace_back(p.x(), p.y());
    height_values.push_back(p.z());
  }
  vgl_point_2d<double> upper_left(scene_box.min_x(), scene_box.max_y());
  size_t ni = height_map_.ni(), nj = height_map_.nj();
  double gsd = params_.point_sample_dist_;
  size_t n_nbr = params_.num_nearest_nbrs_;
  double max_d = params_.max_consist_dist_;
  bpgl_gridding::linear_interp<double, float> interp_fun(max_d, NAN);
  consist_height_map_ = bpgl_gridding::grid_data_2d(triangulated_xy, height_values,
                                                    upper_left, ni, nj, gsd,
                                                    interp_fun, n_nbr);
}

bool bsgm_pairwise_dsm::consistent_pointset()
{
  size_t n = ptset_.size(), n_nbrs = params_.num_nearest_nbrs_;
  if(n == 0)
    return false;
  consist_ptset_.clear();
  bvgl_k_nearest_neighbors_3d<float> knn(ptset_reverse_);
  float davg = 0.0f, dsq = 0.0f, dz_avg = 0.0f, dz_sq = 0.0f;
  float nstat = 0.0f;
  for(size_t i = 0; i<n; ++i){
	  const vgl_point_3d<float>& p = ptset_.p(i);
	  vgl_point_3d<float> cp;
    if(!knn.closest_point(p, cp)){
      std::cout << "KNN index failed to find neighbors - fatal" << std::endl;
      return false;
    }
    float d = vgl_distance(p, cp);
    if(d >params_.max_consist_dist_)
      continue;
    consist_ptset_.add_point(p);
  }
  return true;
}

bool bsgm_pairwise_dsm::compute_consistent_dsm_and_ptset(vgl_box_3d<double> const& scene_box)
{
  vgl_point_3d<double> pmin = scene_box.min_point(), pmax = scene_box.max_point();
  vgl_point_3d<float> pminf(pmin.x(), pmin.y(), pmin.z()), pmaxf(pmax.x(), pmax.y(), pmax.z());
  vgl_box_3d<float> fbox;
  fbox.add(pminf), fbox.add(pmaxf);
  float samp_dist = params_.point_sample_dist_;
  //
  //                              img0               img1     v
  // in sgm disparity estimation img_targ(x,y) <-> img_ref( x + disp_target(x,y), y )
  // the heightmap code requires  img1(u, v) <--> img0(u - disparity, v)
  // thus the order is reversed                          ^
  //
  height_map_ =  bpgl_heightmap_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, scene_box, samp_dist);
  height_map_reverse_ =  bpgl_heightmap_from_disparity(rip_.rect_acam0(), rip_.rect_acam1(), disp_r_reverse_, scene_box, samp_dist);

  std::vector<vgl_point_3d<float> > pts3d, pts3d_reverse;
  bpgl_pointset_from_disparity(rip_.rect_acam1(), rip_.rect_acam0(), disp_r_, fbox, samp_dist, pts3d);
  bpgl_pointset_from_disparity(rip_.rect_acam0(), rip_.rect_acam1(), disp_r_reverse_, fbox, samp_dist, pts3d_reverse);
  ptset_ = vgl_pointset_3d<float>(pts3d);
  ptset_reverse_ = vgl_pointset_3d<float>(pts3d_reverse);
  return this->consistent_pointset();
}
