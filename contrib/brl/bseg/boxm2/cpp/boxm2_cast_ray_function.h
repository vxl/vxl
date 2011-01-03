#ifndef boxm2_cast_ray_function_h_
#define boxm2_cast_ray_function_h_
//:
// \file


#include <vgl/vgl_ray_3d.h>

#include <vul/vul_timer.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

#include <boct/boct_bit_tree.h>

#include <vcl_algorithm.h>
#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f
inline float clamp(float x, float a, float b)
{
    return x < a ? a : (x > b ? b : x);
}


template<class F>
void boxm2_cast_ray_function(vgl_ray_3d<float> & ray,
                             boxm2_scene_info * linfo,
                             boxm2_block_sptr blk_sptr,
                             vcl_vector<float> & vals,
                             F functor)
{
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
  float tfar = vcl_min(vcl_min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->scene_dims[0]) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->scene_dims[1]) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->scene_dims[2]) : 0.0f;
  float tblock = vcl_max(vcl_max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));

  if (tfar <= tblock) {
    return;
  }
  //make sure tnear is at least 0...
  tblock = (tblock > 0.0f) ? tblock : 0.0f;

  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar -= BLOCK_EPSILON;

  int cnt=0;
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
    float cell_minx = clamp(vcl_floor(posx), 0.0f, linfo->scene_dims[0]-1.0f);
    float cell_miny = clamp(vcl_floor(posy), 0.0f, linfo->scene_dims[1]-1.0f);
    float cell_minz = clamp(vcl_floor(posz), 0.0f, linfo->scene_dims[2]-1.0f);

    //load current block/tree
    uchar16 tree=blk_sptr->trees()(cell_minx,cell_miny,cell_minz);

    boct_bit_tree bit_tree((char*)tree.data_block());

    unsigned short buff_index=tree[12];
    buff_index=(buff_index<<8)+tree[13];

    unsigned short sub_index=(short)tree[10];
    sub_index=(sub_index<<8)+(short)tree[11];

    int data_index=(int)buff_index*(int)linfo->tree_buffer_length+(int)sub_index;

//
//    //local ray origin is entry point (point should be in [0,1])
//    //(note that cell_min is the current block index at this point)
//    //setting local_ray_o to block_pos allows ttree to start at 0
    float lrayx = (posx - cell_minx);
    float lrayy = (posy - cell_miny);
    float lrayz = (posz - cell_minz);
//
    //get scene level t exit value.  check to make sure that the ray is progressing.
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    cell_minx = (ray_dx > 0) ? cell_minx+1.0f : cell_minx;
    cell_miny = (ray_dy > 0) ? cell_miny+1.0f : cell_miny;
    cell_minz = (ray_dz > 0) ? cell_minz+1.0f : cell_minz;
    float texit = vcl_min(vcl_min( (cell_minx-ray_ox)*(1.0f/ray_dx), (cell_miny-ray_oy)*(1.0f/ray_dy)), (cell_minz-ray_oz)*(1.0f/ray_dz));
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
//
//      // traverse to leaf cell that contains the entry point, set bounding box
//      ////data offset is ushort pointed to by tree + bit offset

      int bit_index=bit_tree.traverse(vgl_point_3d<double>(posx,posy,posz));
      int depth =bit_tree.depth_at(bit_index);
      float cell_len=vcl_pow((float)2,(float)-depth);

      cell_minx=vcl_floor(posx/cell_len)* cell_len;
      cell_miny=vcl_floor(posy/cell_len)* cell_len;
      cell_minz=vcl_floor(posz/cell_len)* cell_len;

      int data_offset=data_index+bit_tree.get_data_index(bit_index);

      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0.0f) ? cell_minx+cell_len : cell_minx;
      cell_miny = (ray_dy > 0.0f) ? cell_miny+cell_len : cell_miny;
      cell_minz = (ray_dz > 0.0f) ? cell_minz+cell_len : cell_minz;
      float t1 = vcl_min(vcl_min( (cell_minx-lrayx)*(1.0f/ray_dx), (cell_miny-lrayy)*(1.0f/ray_dy)), (cell_minz-lrayz)*(1.0f/ray_dz));

      //make sure ray goes through the cell with positive seg length
      if (t1 <= ttree) break;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;

      functor.step_cell(d,data_index,vals);
    }
    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit + tblock + BLOCK_EPSILON;
    tblock = texit;
  }

}


#endif // boxm2_cast_ray_function_h_
