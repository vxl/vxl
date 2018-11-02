#ifndef boxm2_cast_ray_function_h_
#define boxm2_cast_ray_function_h_
//:
// \file

#include <iostream>
#include <algorithm>
#include <vgl/vgl_ray_3d.h>

#include <cassert>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_util.h>

#include <boct/boct_bit_tree.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_generic_camera.h>

#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f


template<class F>
void boxm2_cast_ray_function(vgl_ray_3d<double> & ray_ij,
                             boxm2_scene_info * linfo,
                             boxm2_block * blk_sptr,
                             unsigned i, unsigned j,
                             F functor, float tfar_max= -1.0f)
{
    vgl_point_3d<float> block_origin(float(ray_ij.origin().x()-linfo->scene_origin[0])/linfo->block_len,
                                            float(ray_ij.origin().y()-linfo->scene_origin[1])/linfo->block_len,
                                            float(ray_ij.origin().z()-linfo->scene_origin[2])/linfo->block_len);

    float dray_ij_x=float(ray_ij.direction().x()),
        dray_ij_y=float(ray_ij.direction().y()),
        dray_ij_z=float(ray_ij.direction().z());

    //thresh ray direction components - too small a treshhold causes axis aligned
    //viewpoints to hang in infinite loop (block loop)
    float thresh = std::exp(-12.0f);
    if (std::fabs(dray_ij_x) < thresh) dray_ij_x = (dray_ij_x>0)?thresh:-thresh;
    if (std::fabs(dray_ij_y) < thresh) dray_ij_y = (dray_ij_y>0)?thresh:-thresh;
    if (std::fabs(dray_ij_z) < thresh) dray_ij_z = (dray_ij_z>0)?thresh:-thresh;

    vgl_vector_3d<float> direction(dray_ij_x,dray_ij_y,dray_ij_z);
    vgl_ray_3d<float> ray(block_origin,direction);

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree

    float ray_dx=ray.direction().x();
    float ray_dy=ray.direction().y();
    float ray_dz=ray.direction().z();

    float ray_ox=ray.origin().x();
    float ray_oy=ray.origin().y();
    float ray_oz=ray.origin().z();

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tfar = std::min(std::min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tblock = std::max(std::max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));


  if (tfar <= tblock || tfar < 0) {
    return;
  }

  //make sure tnear is at least 0...
  tblock = (tblock > 0.0f) ? tblock : 0.0f;
  if(tfar_max >0.0)
    tfar = tfar > tfar_max ? tfar_max: tfar;
  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar -= BLOCK_EPSILON;

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while (tblock < tfar)
  {
    //-------------------------------------------------------------------------
    // get small block and necessary information
    // Note: Can probably eliminate posx,posy,posz and replace with just a calc
    // of position based on ray_o or local ray o and the current t value
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = (ray_ox + (tblock + TREE_EPSILON)*ray_dx);
    float posy = (ray_oy + (tblock + TREE_EPSILON)*ray_dy);
    float posz = (ray_oz + (tblock + TREE_EPSILON)*ray_dz);

    //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
    float cell_minx = boxm2_util::clamp(std::floor(posx), 0.0f, linfo->scene_dims[0]-1.0f);
    float cell_miny = boxm2_util::clamp(std::floor(posy), 0.0f, linfo->scene_dims[1]-1.0f);
    float cell_minz = boxm2_util::clamp(std::floor(posz), 0.0f, linfo->scene_dims[2]-1.0f);

    //load current block/tree
    uchar16 tree=blk_sptr->trees()((unsigned short)cell_minx,(unsigned short)cell_miny,(unsigned short)cell_minz);

    boct_bit_tree bit_tree((unsigned char*)tree.data_block(),linfo->root_level+1);

    //local ray origin is entry point (point should be in [0,1])
    //(note that cell_min is the current block index at this point)
    //setting local_ray_o to block_pos allows ttree to start at 0
    float lrayx = (posx - cell_minx);
    float lrayy = (posy - cell_miny);
    float lrayz = (posz - cell_minz);

    //get scene level t exit value.  check to make sure that the ray is progressing.
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    cell_minx = (ray_dx > 0) ? cell_minx+1.0f : cell_minx;
    cell_miny = (ray_dy > 0) ? cell_miny+1.0f : cell_miny;
    cell_minz = (ray_dz > 0) ? cell_minz+1.0f : cell_minz;
    float texit = std::min(std::min( (cell_minx-ray_ox)*(1.0f/ray_dx), (cell_miny-ray_oy)*(1.0f/ray_dy)), (cell_minz-ray_oz)*(1.0f/ray_dz));
    if (texit <= tblock) break; //need this check to make sure the ray is progressing

    //ttree starts at 0, ttree_exit is t exit value in the tree level (scaled from scene level)
    texit = (texit - tblock - BLOCK_EPSILON);
    float ttree = 0.0f;
    while (ttree < texit)
    {
      // point in tree coordinates
      posx = (lrayx + (ttree + TREE_EPSILON)*ray_dx);
      posy = (lrayy + (ttree + TREE_EPSILON)*ray_dy);
      posz = (lrayz + (ttree + TREE_EPSILON)*ray_dz);

      // traverse to leaf cell that contains the entry point, set bounding box
      //data offset is ushort pointed to by tree + bit offset

      int bit_index=bit_tree.traverse(vgl_point_3d<double>(posx,posy,posz));
      int depth =bit_tree.depth_at(bit_index);
      float cell_len=std::pow((float)2,(float)-depth);

      cell_minx=std::floor(posx/cell_len)* cell_len;
      cell_miny=std::floor(posy/cell_len)* cell_len;
      cell_minz=std::floor(posz/cell_len)* cell_len;

      int data_offset=bit_tree.get_data_index(bit_index);

      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0.0f) ? cell_minx+cell_len : cell_minx;
      cell_miny = (ray_dy > 0.0f) ? cell_miny+cell_len : cell_miny;
      cell_minz = (ray_dz > 0.0f) ? cell_minz+cell_len : cell_minz;
      float t1 = std::min(std::min( (cell_minx-lrayx)*(1.0f/ray_dx), (cell_miny-lrayy)*(1.0f/ray_dy)), (cell_minz-lrayz)*(1.0f/ray_dz));

      //make sure ray goes through the cell with positive seg length
      if (t1 <= ttree) break;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;

      functor.step_cell(d,data_offset,i,j, (ttree + tblock ) * linfo->block_len);
    }

   //scale texit back up
    texit = texit + tblock + BLOCK_EPSILON;
    tblock = texit;
  }
}

template <class functor_type>
bool cast_ray_per_block(functor_type functor,
                        boxm2_scene_info * linfo,
                        boxm2_block * blk_sptr,
                        vpgl_camera_double_sptr cam ,
                        unsigned int roi_ni,
                        unsigned int roi_nj,
                        unsigned int roi_ni0=0,
                        unsigned int roi_nj0=0)
{
  if (vpgl_generic_camera<double>* gcam =
      dynamic_cast<vpgl_generic_camera<double>*>(cam.ptr()))
  {
    for (unsigned i=roi_ni0;i<roi_ni;++i)
    {
      for (unsigned j=roi_nj0;j<roi_nj;++j)
      {
        vgl_ray_3d<double> ray_ij = gcam->ray(i,j);
        boxm2_cast_ray_function<functor_type>(ray_ij,linfo,blk_sptr,i,j,functor);
      }
    }
    return true;
  }
  else if (cam->type_name()== "vpgl_perspective_camera") {
    for (unsigned i=roi_ni0;i<roi_ni;++i) {
      if (i%10==0) std::cout<<'.'<<std::flush;
      for (unsigned j=roi_nj0;j<roi_nj;++j) {
        vgl_ray_3d<double> ray_ij =  ((vpgl_perspective_camera<double>*) cam.ptr())->backproject(i,j);
        boxm2_cast_ray_function<functor_type>(ray_ij,linfo,blk_sptr,i,j,functor);
      }
    }
    return true;
  }

  std::cout<<"boxm2_cast_ray_function cannot dynamic cast camera"<<std::endl;
  return false;
}


#endif // boxm2_cast_ray_function_h_
