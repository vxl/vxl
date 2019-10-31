// This is contrib/brl/bbas/bpgl/algo/bpgl_heightmap_from_disparity.h
#ifndef bpgl_heightmap_from_disparity_h_
#define bpgl_heightmap_from_disparity_h_
//:
// \file
// \brief Methods related to z(x,y) heightmap generation
// \author Dan Crispell
// \date Nov 15, 2018
//

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vil/vil_image_view.h>


/**
 * Main convenience function
 * Given cameras for a stereo pair and a disparity map, return a 3-plane (x,y,z)
 * image containing the corresponding triangulated 3D points in the form z(x,y)
 * disparity should contain pixel offsets such that (img1(u, v) <--> img2(u + disparity, v)
 * for example, given the pixel location (100, 200) in image1, and a disparity value of -20 at that location
 * in the disparity image,x then the corresponding pixel location in image2 is (80, 200).
 */
template<class T, class CAM_T>
vil_image_view<T> bpgl_heightmap_from_disparity(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity,
    vgl_box_3d<T> heightmap_bounds,
    T ground_sample_distance);


/**
 * Helper class, separating each step for fine grained use
 * Given a triangulated image returned from "bpgl_3d_from_disparity",
 * convert to pointsets and/or heightmaps as appropriate.
 **/
template<class T>
class bpgl_heightmap
{
  public:

    //: constructor
    bpgl_heightmap() = default;

    bpgl_heightmap(
        vgl_box_3d<T> heightmap_bounds,
        T ground_sample_distance) :
      _heightmap_bounds(heightmap_bounds),
      _ground_sample_distance(ground_sample_distance)
    {}

    //: destructor
    ~bpgl_heightmap() = default;

    // Accessors
    T ground_sample_distance() const { return _ground_sample_distance; }
    void set_ground_sample_distance(T ground_sample_distance) {
      _ground_sample_distance = ground_sample_distance;
    }

    vgl_box_3d<T> heightmap_bounds() const { return _heightmap_bounds; }
    void set_heightmap_bounds(vgl_box_3d<T> heightmap_bounds) {
      _heightmap_bounds = heightmap_bounds;
    }

    T neighbor_dist_factor() const { return _neighbor_dist_factor; }
    void set_neighbor_dist_factor(T neighbor_dist_factor) {
      _neighbor_dist_factor = neighbor_dist_factor;
    }

    unsigned num_neighbors() const { return _num_neighbors; }
    void set_num_neighbors(unsigned num_neighbors) {
      _num_neighbors = num_neighbors;
    }

    //: compute pointset from triangulated image
    void pointset_from_tri(
        const vil_image_view<T>& tri_3d,
        vgl_pointset_3d<T>& ptset_output);

    //: compute pointset from triangulated image & scalar input
    void pointset_from_tri(
        const vil_image_view<T>& tri_3d,
        const vil_image_view<T>& scalar,
        vgl_pointset_3d<T>& ptset_output);

    //: compute heightmap from pointset input
    void heightmap_from_pointset(
        const vgl_pointset_3d<T>& ptset,
        vil_image_view<T>& heightmap_output);

    //: compute heightmap & scalar field from pointset input
    void heightmap_from_pointset(
        const vgl_pointset_3d<T>& ptset,
        vil_image_view<T>& heightmap_output,
        vil_image_view<T>& scalar_output);

    //: compute heightmap from triangulated image
    void heightmap_from_tri(
        const vil_image_view<T>& tri_3d,
        vil_image_view<T>& heightmap_output);

    //: compute heightmap & scalar field from triangulated image & scalar input
    void heightmap_from_tri(
        const vil_image_view<T>& tri_3d,
        const vil_image_view<T>& scalar,
        vil_image_view<T>& heightmap_output,
        vil_image_view<T>& scalar_output);

  private:

    // compute pointset from triangulated image & scalar input
    // scalar usage controlled by "ignore_scalar" flag
    void _pointset_from_tri(
        const vil_image_view<T>& tri_3d,
        const vil_image_view<T>& scalar,
        vgl_pointset_3d<T>& ptset_output,
        bool ignore_scalar);

    // compute heightmap & scalar field from pointset input
    // scalar usage controlled by "ignore_scalar" flag
    void _heightmap_from_pointset(
        const vgl_pointset_3d<T>& ptset,
        vil_image_view<T>& heightmap_output,
        vil_image_view<T>& scalar_output,
        bool ignore_scalar);

    // parameters
    vgl_box_3d<T> _heightmap_bounds;
    T _ground_sample_distance;

    // gridding parameters
    T _neighbor_dist_factor = 3.0;
    unsigned _num_neighbors = 3;

};

#endif
