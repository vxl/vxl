#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
#define EPSILON .0125
                       
__kernel
void
update_bit_scene_opt(__constant  RenderSceneInfo    * linfo,
                     __global    ushort2            * block_ptrs,
                     __global    int4               * tree_array,       // tree structure for each block
                     __global    float              * alpha_array,      // alpha for each block
                     __global    uchar8             * mixture_array,    // mixture for each block
                     __global    ushort4            * num_obs_array,    // num obs for each block
                     __global    float4             * aux_data_array,   // aux data used between passes
                     __constant  uchar              * bit_lookup,       // used to get data_index
                     __local     uchar16            * local_tree,       // cache current tree into local memory
                     __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
                     __global    uint4              * imgdims,          // dimensions of the input image
                     __global    float4             * in_image,         // the input image
                     __global    int                * offsetfactor,     // 
                     __global    int                * offset_x,         // offset to the left and 
                     __global    int                * offset_y,         // right (which one of the four blocks)
                     __local     uchar4             * ray_bundle_array, //gives information for which ray takes over in the workgroup
                     __local     int                * cell_ptrs,        //local list of cell_ptrs (cells that are hit by this workgroup
                     __local     float16            * cached_data,      //
                     __local     float4             * cached_aux_data,
                     __local     float4             * image_vect,       // input image and store vis_inf and pre_inf
                     __global    float              * output)    
{

  //get local id (0-63 for an 8x8) of this patch 
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //----------------------------------------------------------------------------
  // get image coordinates and camera, 
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0; map_work_space_2d_offset(&i,&j,(*offset_x),(*offset_y));

  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  int factor=(*offsetfactor);
  image_vect[llid] = in_image[j*get_global_size(0)*factor+i];
  barrier(CLK_LOCAL_MEM_FENCE);

  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  // ray origin, ray direction, inverse ray direction (make sure ray direction is never axis aligned)
  float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned 
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f); 
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0; ray_d = normalize(ray_d);
  
  //store float 3's
  float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
  float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;
  float inv_x=1.0f/ray_dx;    float inv_y=1.0f/ray_dy;    float inv_z=1.0f/ray_dz;
  // pixel values/depth map to be returned
  float vis = 1.0f;
  float expected_int = 0.0f;
  float intensity_norm = 0.0f;

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0) ? (linfo->dims.x) : 0.0;
  float max_facey = (ray_dy > 0.0) ? (linfo->dims.y) : 0.0;
  float max_facez = (ray_dz > 0.0) ? (linfo->dims.z) : 0.0;
  float tfar = min(min( (max_facex-ray_ox)*(1.0/ray_dx), (max_facey-ray_oy)*(1.0/ray_dy)), (max_facez-ray_oz)*(1.0/ray_dz));
  float min_facex = (ray_dx < 0.0) ? (linfo->dims.x) : 0.0;
  float min_facey = (ray_dy < 0.0) ? (linfo->dims.y) : 0.0;
  float min_facez = (ray_dz < 0.0) ? (linfo->dims.z) : 0.0;
  float tblock = max(max( (min_facex-ray_ox)*(1.0/ray_dx), (min_facey-ray_oy)*(1.0/ray_dy)), (min_facez-ray_oz)*(1.0/ray_dz));
  if (tfar <= tblock) {
    return;
  }
  //make sure tnear is at least 0...
  tblock = (tblock > 0) ? tblock : 0;
  
  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar = tfar - EPSILON;   
  
  //used for depth map 
  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while(tblock < tfar) 
  {
    //-------------------------------------------------------------------------
    // get small block and necessary information
    // Note: Can probably eliminate posx,posy,posz and replace with just a calc
    // of position based on ray_o or local ray o and the current t value
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = (ray_ox + (tblock + linfo->epsilon)*ray_dx);
    float posy = (ray_oy + (tblock + linfo->epsilon)*ray_dy);
    float posz = (ray_oz + (tblock + linfo->epsilon)*ray_dz);
    
    //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
    float cell_minx = clamp(floor(posx), 0.0, linfo->dims.x-1.0);
    float cell_miny = clamp(floor(posy), 0.0, linfo->dims.y-1.0);
    float cell_minz = clamp(floor(posz), 0.0, linfo->dims.z-1.0);

    //load current block/tree 
    ushort2 block = block_ptrs[convert_int(cell_minz + (cell_miny + cell_minx*linfo->dims.y)*linfo->dims.z)];
    int root_ptr = block.x * linfo->tree_len + block.y;
    local_tree[llid] = as_uchar16(tree_array[root_ptr]);
    barrier(CLK_LOCAL_MEM_FENCE);

    //local ray origin is entry point (point should be in [0,1]) 
    //(note that cell_min is the current block index at this point)
    //setting local_ray_o to block_pos allows ttree to start at 0
    float lrayx = (posx - cell_minx);
    float lrayy = (posy - cell_miny);
    float lrayz = (posz - cell_minz);
    
    //get scene level t exit value.  check to make sure that the ray is progressing. 
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    cell_minx = (ray_dx > 0) ? cell_minx+1.0 : cell_minx; 
    cell_miny = (ray_dy > 0) ? cell_miny+1.0 : cell_miny; 
    cell_minz = (ray_dz > 0) ? cell_minz+1.0 : cell_minz; 
    float texit = min(min( (cell_minx-ray_ox)*(1.0/ray_dx), (cell_miny-ray_oy)*(1.0/ray_dy)), (cell_minz-ray_oz)*(1.0/ray_dz));
    if(texit <= tblock) break; //need this check to make sure the ray is progressing

    //ttree starts at 0, ttree_exit is t exit value in the tree level (scaled from scene level)
    texit = (texit - tblock - EPSILON);
    float ttree = 0.0;
#if 1
    while (ttree < texit)
    {
      // point in tree coordinates
      posx = (lrayx + (ttree + EPSILON)*ray_dx);
      posy = (lrayy + (ttree + EPSILON)*ray_dy);
      posz = (lrayz + (ttree + EPSILON)*ray_dz);
      
      // traverse to leaf cell that contains the entry point, set bounding box
      ////data offset is ushort pointed to by tree + bit offset
      float cell_len;
      int data_ptr = traverse_three((llid<<4), local_tree, 
                                    posx,posy,posz, 
                                    &cell_minx, &cell_miny, &cell_minz, &cell_len);
      if(data_ptr != 0) {
        output[0] = -666;
        output[1] = -666;
      } 
      data_ptr = data_index_opt( (llid<<4), local_tree, data_ptr, bit_lookup);
      data_ptr = block.x * linfo->data_len + data_ptr;
      
      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0) ? cell_minx+cell_len : cell_minx; 
      cell_miny = (ray_dy > 0) ? cell_miny+cell_len : cell_miny; 
      cell_minz = (ray_dz > 0) ? cell_minz+cell_len : cell_minz;
      float t1 = min(min( (cell_minx-lrayx)*inv_x, (cell_miny-lrayy)*inv_y), (cell_minz-lrayz)*inv_z);

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;
      
      //if cell dist is less than zero, break out of tree loop
      if(d <= 0) break;

      //keep track of cells being hit
      cell_ptrs[llid] = data_ptr;
      ray_bundle_array[llid].x=llid;
      barrier(CLK_LOCAL_MEM_FENCE);

      ////////////////////////////////////////////////////////
      // the place where the ray trace function can be applied
      load_data_mutable_using_cell_ptrs(ray_bundle_array,cell_ptrs);
      if (ray_bundle_array[llid].x==llid)
      {
          //      /* cell data, i.e., alpha and app model is needed for some passes */
#if %%
          float  alpha    = alpha_array[data_ptr];
          float4 nobs     = convert_float4(num_obs_array[data_ptr]);
          float8 mixture  = convert_float8(mixture_array[data_ptr]);
          float16 datum = (float16) (alpha, 
                         (mixture.s0/255.0), (mixture.s1/255.0), (mixture.s2/255.0), (nobs.s0), 
                         (mixture.s3/255.0), (mixture.s4/255.0), (mixture.s5/255.0), (nobs.s1),
                         (mixture.s6/255.0), (mixture.s7/255.0), (nobs.s2), (nobs.s3/100.0), 
                         0.0, 0.0, 0.0);
          cached_data[llid] = datum;
#endif
          cached_aux_data[llid] = aux_data_array[data_ptr];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      
      /**********************************************
       * no function pointers in OpenCL (spec 8.6a)
       * instead, user must provide source with a function named "step_cell" */
      $$step_cell$$;
      /*********************************************/
      
      if (ray_bundle_array[llid].x==llid)
      {
          /* note that sample data is not changed during ray tracing passes */
          aux_data_array[data_ptr]=(float4)cached_aux_data[llid] ;
      }
      in_image[j*get_global_size(0)*factor+i] = image_vect[llid];


    }
#endif

    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit + tblock + EPSILON;
    tblock = texit;
  }

}



__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global uchar8           * mixture_array,
                      __global ushort4          * nobs_array,
                      __global float4           * aux_data_array,
                      __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len * info->num_buffer;
    if (gid<datasize)
    {
      //if alpha is less than zero don't update
      float  alpha    = alpha_array[gid];
      if(alpha <= 0.0) return;
      
      //load global data into registers
      float4 aux_data = aux_data_array[gid];
      float4 nobs     = convert_float4(nobs_array[gid]);
      float8 mixture  = convert_float8(mixture_array[gid]);
      float16 data = (float16) (alpha, 
                     (mixture.s0/255.0), (mixture.s1/255.0), (mixture.s2/255.0), (nobs.s0), 
                     (mixture.s3/255.0), (mixture.s4/255.0), (mixture.s5/255.0), (nobs.s1),
                     (mixture.s6/255.0), (mixture.s7/255.0), (nobs.s2), (nobs.s3/100.0), 
                     0.0, 0.0, 0.0);
      
      //use aux data to update cells 
      if (aux_data.x>1e-10f)
        update_cell(&data, aux_data, 2.5f, 0.09f, 0.03f);

      //reset the cells in memory 
      alpha_array[gid]      = data.s0;
      float8 post_mix       = (float8) (data.s1, data.s2, data.s3, 
                                        data.s5, data.s6, data.s7, 
                                        data.s9, data.sa)*255.0;
      float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
      mixture_array[gid]    = convert_uchar8_sat_rte(post_mix);
      nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);      
      aux_data_array[gid]   = (float4)0.0f;
    }
    
#if 1 
    if(gid==345) {
      output[0] = info->data_len; 
      output[1] = info->num_buffer;
/*
      output[0] = alpha_array[gid];
      output[1] = (float) (mixture_array[gid].s0/255.0); //mu0
      output[2] = (float) (mixture_array[gid].s1/255.0); //sig0
      output[3] = (float) (mixture_array[gid].s2/255.0); //w0
      output[4] = (float) (mixture_array[gid].s3/255.0); //mu1
      output[5] = (float) (mixture_array[gid].s4/255.0); //sig1
      output[6] = (float) (mixture_array[gid].s5/255.0); //w1
      output[7] = (float) (mixture_array[gid].s6/255.0); //mu2
      output[8] = (float) (mixture_array[gid].s7/255.0); //sig2

      output[9] = (float) (nobs_array[gid].s3/100.0);
*/
    }
#endif
    
}



