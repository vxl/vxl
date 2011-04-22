#ifndef boxm2_cast_cone_ray_function_h_
#define boxm2_cast_cone_ray_function_h_
//:
// \file

#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_intersection.h>

#include <bvgl/bvgl_volume_of_intersection.h>

#include <vul/vul_timer.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_util.h>
#include <boct/boct_bit_tree2.h>
#include <vcl_algorithm.h>

//camera includes
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>

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
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  double sinAlpha = vcl_sin(cone_half_angle);

  // If ray origin is inside box then p0==p1
  float ray_dx=(float)ray.direction().x();
  float ray_dy=(float)ray.direction().y();
  float ray_dz=(float)ray.direction().z();
  float ray_ox=(float)ray.origin().x();
  float ray_oy=(float)ray.origin().y();
  float ray_oz=(float)ray.origin().z();

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tfar = vcl_min(vcl_min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tblock = vcl_max(vcl_max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));
  if (tfar <= tblock) {
    return;
  }

  //make sure tnear is at least 0...
  double currT = (double) vcl_max( (double) tblock, MIN_T);

  //calculate t far
  double tFar = (double) tfar;

  //curr radius
  double currR = currT * sinAlpha;

  //iterate over spheres
  while (currT < tFar)
  {
    //if this pixel is no longer visible, quit
    if (functor.vis(i,j) < .01) return;

    //intersect the current sphere with
    vgl_sphere_3d<double> currSphere( ray.origin() + ray.direction() * currT, currR);

    //minimum/maximum subblock eclipsed
    vgl_point_3d<int> minCell( (int) (currSphere.centre().x() - currR),
                               (int) (currSphere.centre().y() - currR),
                               (int) (currSphere.centre().z() - currR) );
    vgl_point_3d<int> maxCell( (int) vcl_min( (int) (currSphere.centre().x() + currR + 1.0), linfo->scene_dims[0] ),
                               (int) vcl_min( (int) (currSphere.centre().y() + currR + 1.0), linfo->scene_dims[1] ),
                               (int) vcl_min( (int) (currSphere.centre().z() + currR + 1.0), linfo->scene_dims[2] ) );

    float intensity_norm = 0.0f;
    float weighted_int = 0.0f;
    float prob_surface = 0.0f, total_volume = 0.0f;
    for (int x=minCell.x(); x<maxCell.x(); ++x) {
      for (int y=minCell.y(); y<maxCell.y(); ++y) {
        for (int z=minCell.z(); z<maxCell.z(); ++z) {
          //load current block/tree
          uchar16 tree = blk_sptr->trees()(x,y,z);
          boct_bit_tree2 bit_tree( (unsigned char*)tree.data_block(), linfo->root_level+1);
          int data_ptr = bit_tree.get_data_ptr();

#if 0
          //calculate contribution from each subblock's voxels
          //if the current raysphere diameter is less than cell length, traverse
          if ( 2.0*currR < linfo->block_len ) {
            float cell_len = linfo->block_len;
            while ( 2.0*currR < cell_len )
            {
            }
          }
#endif

          //calculate the theoretical radius of this cell
          double cellR = UNIT_SPHERE_RADIUS;
          vgl_point_3d<double> cellCenter( (double) x + 0.5,
                                           (double) y + 0.5,
                                           (double) z + 0.5 );
          vgl_sphere_3d<double> cellSphere(cellCenter, cellR);
          double intersect_volume = bvgl_volume_of_intersection(currSphere, cellSphere);

          //call step cell
          functor.step_cell(intersect_volume, data_ptr, i, j, linfo->block_len,
                            intensity_norm, weighted_int, prob_surface);
          total_volume += intersect_volume;
        }
      }
    }

    //calculate ray/sphere occupancy prob
    float sphere_occ_prob = prob_surface/total_volume;

    //update intensity
    if (intensity_norm > 1e-10 && total_volume > 1e-10) {
      functor.update_expected_int( weighted_int/intensity_norm, sphere_occ_prob, i, j );

      //update visibility after all cells have accounted for
      functor.update_vis( sphere_occ_prob, i, j);
    }

    //calculate the next sphere's R and T
    float rPrime = sinAlpha * (currR + currT) / (1.0-sinAlpha);
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
      if (i%10==0) vcl_cout<<'.'<<vcl_flush;
      for (unsigned j=roi_nj0;j<roi_nj;++j)
      {
        //calculate ray and ray angles at pixel ij
        vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
        double cone_half_angle, solid_angle;
        vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
        vpgl_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);

        //normalize ray such that each block is of unit length
        vgl_point_3d<double> block_origin( (ray_ij.origin().x()-linfo->scene_origin[0])/linfo->block_len,
                                           (ray_ij.origin().y()-linfo->scene_origin[1])/linfo->block_len,
                                           (ray_ij.origin().z()-linfo->scene_origin[2])/linfo->block_len);

        //thresh ray direction components - too small a treshhold causes axis aligned
        //viewpoints to hang in infinite loop (block loop)
        double dray_ij_x=double(ray_ij.direction().x()),
               dray_ij_y=double(ray_ij.direction().y()),
               dray_ij_z=double(ray_ij.direction().z());
        double thresh = vcl_exp(-12.0f);
        if (vcl_fabs(dray_ij_x) < thresh) dray_ij_x = (dray_ij_x>0)?thresh:-thresh;
        if (vcl_fabs(dray_ij_y) < thresh) dray_ij_y = (dray_ij_y>0)?thresh:-thresh;
        if (vcl_fabs(dray_ij_z) < thresh) dray_ij_z = (dray_ij_z>0)?thresh:-thresh;

        //calculate vgl box 3d
        vgl_point_3d<double> minCorner(0.0, 0.0, 0.0);
        vgl_point_3d<double> maxCorner( (double) linfo->scene_dims[0],
                                        (double) linfo->scene_dims[0],
                                        (double) linfo->scene_dims[0] );
        vgl_box_3d<double> block_box(minCorner, maxCorner);

        //cast cone ray function
        vgl_vector_3d<double> direction(dray_ij_x,dray_ij_y,dray_ij_z);
        vgl_ray_3d<double> norm_ray_ij(block_origin, direction);
        boxm2_cast_cone_ray_function<functor_type>(block_box, norm_ray_ij, cone_half_angle, linfo, blk_sptr, i, j, functor);
      }
    }
    return true;
  }
  vcl_cout<<"Cast Cone Ray Per Block Returning False"<<vcl_endl;
  return false;
}


#endif // boxm2_cast_cone_ray_function_h_
