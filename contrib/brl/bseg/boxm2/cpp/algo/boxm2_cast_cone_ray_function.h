#ifndef boxm2_cast_cone_ray_function_h_
#define boxm2_cast_cone_ray_function_h_
//:
// \file

#include <iostream>
#include <algorithm>
#include <list>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_ray_3d.h>

#include <bvgl/bvgl_volume_of_intersection.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_util.h>
#include <boct/boct_bit_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// camera includes
#include <vpgl/vpgl_perspective_camera.h>
#include <vsph/vsph_camera_bounds.h>

#define MIN_T         1.0
#define UNIT_SPHERE_RADIUS 0.620350490899400016668006812 // = 1/vcl_cbrt(vnl_math::pi*4/3);

//: Return true if ray intersects box. If so, compute intersection points.
template<class F>
void boxm2_cast_cone_ray_function(vgl_box_3d<double>& block_box,
                                  vgl_ray_3d<double>& ray,
                                  double cone_half_angle,
                                  boxm2_scene_info * linfo,
                                  boxm2_block * blk_sptr,
                                  unsigned i, unsigned j,
                                  F functor)
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    // defines a bit tree
  double sinAlpha = std::sin(cone_half_angle);


  float volume_scale=linfo->block_len*linfo->block_len*linfo->block_len;
  // If ray origin is inside box then p0==p1
  float ray_dx=(float)ray.direction().x();
  float ray_dy=(float)ray.direction().y();
  float ray_dz=(float)ray.direction().z();
  float ray_ox=(float)ray.origin().x();
  float ray_oy=(float)ray.origin().y();
  float ray_oz=(float)ray.origin().z();

  // determine the minimum face:
  // get parameters tNear and tFar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tfar = std::min(std::min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tblock = std::max(std::max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));
  if (tfar <= tblock) {
    return;
  }

  // make sure tNear is at least 0...
  double currT = (double) std::max( (double) tblock, MIN_T);

  // calculate tFar
  double tFar = (double) tfar;

  // curr radius
  double currR = currT * sinAlpha;

  // iterate over spheres
  while (currT < tFar)
  {
#if 0
    // if this pixel is no longer visible, quit
    if (functor.vis(i,j) < .01) return;
#endif
    // intersect the current sphere with
    vgl_sphere_3d<double> currSphere( ray.origin() + ray.direction() * currT, currR);

    // minimum/maximum subblock eclipsed
    vgl_point_3d<int> minCell( (int) std::max( (int) (currSphere.centre().x() - currR), 0),
                               (int) std::max( (int) (currSphere.centre().y() - currR), 0),
                               (int) std::max( (int) (currSphere.centre().z() - currR), 0) );
    vgl_point_3d<int> maxCell( (int) std::min( (int) (currSphere.centre().x() + currR + 1.0), linfo->scene_dims[0] ),
                               (int) std::min( (int) (currSphere.centre().y() + currR + 1.0), linfo->scene_dims[1] ),
                               (int) std::min( (int) (currSphere.centre().z() + currR + 1.0), linfo->scene_dims[2] ) );

    for (int x=minCell.x(); x<maxCell.x(); ++x) {
      for (int y=minCell.y(); y<maxCell.y(); ++y) {
        for (int z=minCell.z(); z<maxCell.z(); ++z) {
          // load current block/tree
          uchar16 tree = blk_sptr->trees()(x,y,z);
          boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), linfo->root_level+1);

          // determine how deep in each block you'll go
          unsigned deepest_gen = linfo->root_level;

          // max cell - go through deepest generation
          int max_cell = ((1 << (3*(deepest_gen+1))) - 1) / 7;
          max_cell = std::min(585, max_cell);

          // depth first search (DFS) through the tree
          std::list<unsigned> toVisit;
          toVisit.push_back( 0 );
          while ( !toVisit.empty() ) // DFS
          {
            // pop current node off the top of the list
            unsigned currBitIndex = toVisit.front();
            toVisit.pop_front();

            // calculate the theoretical radius of this cell
            int curr_depth = bit_tree.depth_at(currBitIndex);
            double side_len = 1.0 / (double) (1<<curr_depth);
            double cellR = UNIT_SPHERE_RADIUS * side_len;

            // intersect the cell,
            vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
            vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
            vgl_sphere_3d<double> cellSphere(cellCenter, cellR);
            double intersect_volume = bvgl_volume_of_intersection(currSphere, cellSphere);

            // if it intersects, do one of two things
            if ( intersect_volume > 0 ) {
              // if the tree is a leaf, then update its contribution
              if ( bit_tree.is_leaf(currBitIndex)) {
                int data_ptr = bit_tree.get_data_index(currBitIndex);
                functor.step_cell(static_cast<float>(intersect_volume)*volume_scale,data_ptr, i, j);
              }
              else { // add children to the visit list
                unsigned firstChild = 8 * currBitIndex + 1;
                for (int ci = 0; ci < 8; ++ci)
                  toVisit.push_back( firstChild + ci );
              }
            }
          } //end DFS while
        } //end z for
      } //end y for
    } //end x for

    functor.compute_ball_properties(i,j);
    // calculate ray/sphere occupancy prob
    for (int x=minCell.x(); x<maxCell.x(); ++x) {
      for (int y=minCell.y(); y<maxCell.y(); ++y) {
        for (int z=minCell.z(); z<maxCell.z(); ++z) {
          // load current block/tree
          uchar16 tree = blk_sptr->trees()(x,y,z);
          boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), linfo->root_level+1);

          // determine how deep in each block you'll go
          unsigned deepest_gen = linfo->root_level;

          // max cell - go through deepest generation
          int max_cell = ((1 << (3*(deepest_gen+1))) - 1) / 7;
          max_cell = std::min(585, max_cell);

          // depth first search (DFS) through the tree
          std::list<unsigned> toVisit;
          toVisit.push_back( 0 );
          while ( !toVisit.empty() ) // DFS
          {
            // pop current node off the top of the list
            unsigned currBitIndex = toVisit.front();
            toVisit.pop_front();

            // calculate the theoretical radius of this cell
            int curr_depth = bit_tree.depth_at(currBitIndex);
            double side_len = 1.0 / (double) (1<<curr_depth);
            double cellR = UNIT_SPHERE_RADIUS * side_len;

            // intersect the cell
            vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
            vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
            vgl_sphere_3d<double> cellSphere(cellCenter, cellR);
            double intersect_volume = bvgl_volume_of_intersection(currSphere, cellSphere);

            // if it intersects, do one of two things
            if ( intersect_volume > 0 ) {
              // if the tree is a leaf, then update its contribution
              if ( bit_tree.is_leaf(currBitIndex)) {
                int data_ptr = bit_tree.get_data_index(currBitIndex);
                functor.redistribute(static_cast<float>(intersect_volume)*volume_scale,data_ptr);
              }
              else { // add children to the visit list
                unsigned firstChild = 8 * currBitIndex + 1;
                for (int ci = 0; ci < 8; ++ci)
                  toVisit.push_back( firstChild + ci );
              }
            }
          } //end DFS while
        } //end z for
      } //end y for
    } //end x for
    // calculate the next sphere's R and T
    float rPrime = static_cast<float>(sinAlpha * (currR + currT) / (1.0-sinAlpha));
    currT += (rPrime + currR);
    currR = rPrime;
  }
}

template <class functor_type>
bool cast_cone_ray_per_block( functor_type functor,
                              boxm2_scene_info * linfo,
                              boxm2_block * blk_sptr,
                              vpgl_camera_double_sptr cam ,
                              unsigned int roi_ni,
                              unsigned int roi_nj,
                              unsigned int roi_ni0=0,
                              unsigned int roi_nj0=0)
{
  if (dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
  {
    for (unsigned i=roi_ni0;i<roi_ni;++i)
    {
      if (i%10==0) std::cout<<'.'<<std::flush;
      for (unsigned j=roi_nj0;j<roi_nj;++j)
      {
        // calculate ray and ray angles at pixel ij
        vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
        double cone_half_angle, solid_angle;
        vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
        vsph_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);

        // normalize ray such that each block is of unit length
        vgl_point_3d<double> block_origin( (ray_ij.origin().x()-linfo->scene_origin[0])/linfo->block_len,
                                           (ray_ij.origin().y()-linfo->scene_origin[1])/linfo->block_len,
                                           (ray_ij.origin().z()-linfo->scene_origin[2])/linfo->block_len);

        // thresh ray direction components - too small a treshhold causes axis aligned
        // viewpoints to hang in infinite loop (block loop)
        double dray_ij_x=double(ray_ij.direction().x()),
               dray_ij_y=double(ray_ij.direction().y()),
               dray_ij_z=double(ray_ij.direction().z());
        double thresh = std::exp(-12.0f);
        if (std::fabs(dray_ij_x) < thresh) dray_ij_x = (dray_ij_x>0)?thresh:-thresh;
        if (std::fabs(dray_ij_y) < thresh) dray_ij_y = (dray_ij_y>0)?thresh:-thresh;
        if (std::fabs(dray_ij_z) < thresh) dray_ij_z = (dray_ij_z>0)?thresh:-thresh;

        // calculate vgl_box_3d
        vgl_point_3d<double> minCorner(0.0, 0.0, 0.0);
        vgl_point_3d<double> maxCorner( (double) linfo->scene_dims[0],
                                        (double) linfo->scene_dims[0],
                                        (double) linfo->scene_dims[0] );
        vgl_box_3d<double> block_box(minCorner, maxCorner);

        // cast cone ray function
        vgl_vector_3d<double> direction(dray_ij_x,dray_ij_y,dray_ij_z);
        vgl_ray_3d<double> norm_ray_ij(block_origin, direction);
        boxm2_cast_cone_ray_function<functor_type>(block_box, norm_ray_ij, cone_half_angle, linfo, blk_sptr, i, j, functor);
      }
    }
    return true;
  }
  std::cout<<"Cast Cone Ray Per Block Returning False"<<std::endl;
  return false;
}


#endif // boxm2_cast_cone_ray_function_h_
