// This is brl/bseg/bvpl/bvpl_direction_to_color_map.h
#ifndef bvpl_direction_to_color_map_h_
#define bvpl_direction_to_color_map_h_
//:
// \file
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bdgl/bdgl_peano_curve.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

#include <bxml/bsvg/bsvg_document.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_find.h>
#include <bxml/bxml_write.h>

#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct point_3d_cmp
{
  bool operator()(vgl_point_3d<double> p1, vgl_point_3d<double> p2) const
  {
    if (std::abs(p2.x()-p1.x()) > std::numeric_limits<double>::epsilon())
      return p1.x() < p2.x();
    else if (std::abs(p2.y()-p1.y()) > std::numeric_limits<double>::epsilon())
      return p1.y() < p2.y();
    else
      return p2.z()-p1.z() > std::numeric_limits<double>::epsilon();
  }
};

class bvpl_direction_to_color_map
{
 public:
  bvpl_direction_to_color_map(std::vector<vgl_point_3d<double> > samples,const std::string& type);
  ~bvpl_direction_to_color_map()= default;
  void make_svg_color_map(const std::string& outfile);
  float get_color(vgl_point_3d<double> dir){return colors_[dir];}
 private:
  std::vector<vgl_point_3d<double> > samples_;
  std::map<vgl_point_3d<double>,float,point_3d_cmp>  colors_;
  void project_sphereical_samples_to_cubes(std::vector<vgl_point_3d<double> > & proj_on_cube);
  std::vector<float> find_closest_points_from_cube_to_peano_curve(std::vector<vgl_point_3d<double> > peano_curve,
                                                                 std::vector<vgl_point_3d<double> > proj_on_cube);
};

//: function to map direction vectors to a color map.
void bvpl_generate_direction_samples_from_kernels(const bvpl_kernel_vector_sptr& kernel_vector,
                                                  std::vector<vgl_point_3d<double> > & samples);

void bvpl_convert_grid_to_hsv_grid(bvxm_voxel_grid<vnl_float_4 > *grid,
                                   bvxm_voxel_grid<vnl_float_4 > *out_grid,
                                   bvpl_direction_to_color_map & color_map);

void bvpl_convert_id_grid_to_hsv_grid(bvxm_voxel_grid<int> *id_grid,
                                      bvxm_voxel_grid<float> *response_grid,
                                      bvxm_voxel_grid<vnl_float_4> *out_grid,
                                      std::vector<float> colors);

void bvpl_convert_id_grid_to_hsv_grid(bvxm_voxel_grid<int> *id_grid,
                                      bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *response_grid,
                                      bvxm_voxel_grid<vnl_float_4> *out_grid,
                                      std::vector<float> colors);

//: Function to write a vector of hues, to an svg file.
//  This function is useful, when the hue is identified by an id (index in the vector)
void bvpl_write_colors_to_svg(const bvpl_kernel_vector_sptr& kernel_vector,std::vector<float> hue_vector, const std::string& outfile);

#endif // bvpl_direction_to_color_map_h_
