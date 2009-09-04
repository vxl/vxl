// This is brl/bseg/bvpl/bvpl_direction_to_color_map.h
#ifndef bvpl_direction_to_color_map_h_
#define bvpl_direction_to_color_map_h_
//:
// \file
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvpl/bvpl_kernel_factory.h>
#include <bdgl/bdgl_peano_curve.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vil/algo/vil_colour_space.h>

#include <bxml/bsvg/bsvg_document.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_find.h>
#include <bxml/bxml_write.h>
#include <vcl_iomanip.h>
#include <vcl_limits.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
struct point_3d_cmp
{
  bool operator()(vgl_point_3d<double> p1, vgl_point_3d<double> p2) const
  {
    if (vcl_abs(p2.x()-p1.x()) > vcl_numeric_limits<double>::epsilon())
      return p1.x() < p2.x();
    else if (vcl_abs(p2.y()-p1.y()) > vcl_numeric_limits<double>::epsilon())
      return p1.y() < p2.y();
    else
      return p2.z()-p1.z() > vcl_numeric_limits<double>::epsilon();
  }
};

class bvpl_direction_to_color_map
{
public:
    bvpl_direction_to_color_map(vcl_vector<vgl_point_3d<double> > samples,vcl_string type);
    ~bvpl_direction_to_color_map(){}
    void make_svg_color_map(vcl_string outfile);
    float get_color(vgl_point_3d<double> dir){return colors_[dir];}
private:
    vcl_vector<vgl_point_3d<double> > samples_;
    vcl_map<vgl_point_3d<double>,float,point_3d_cmp>  colors_;
    void project_sphereical_samples_to_cubes(vcl_vector<vgl_point_3d<double> > & proj_on_cube);
    vcl_vector<float> find_closest_points_from_cube_to_peano_curve(vcl_vector<vgl_point_3d<double> > peano_curve,
        vcl_vector<vgl_point_3d<double> > proj_on_cube);




    };
//: function to map direction vectors to a color map.
void bvpl_generate_direction_samples_from_kernels(bvpl_kernel_vector_sptr kernel_vector,
                                                  vcl_vector<vgl_point_3d<double> > & samples);


void bvpl_convert_grid_to_hsv_grid(bvxm_voxel_grid<vnl_float_4 > *grid,
                                   bvxm_voxel_grid<vnl_float_4 > *out_grid,
                                   bvpl_direction_to_color_map & color_map);
#endif // bvpl_direction_to_color_map_h_
