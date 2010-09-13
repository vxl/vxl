#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#define EPSILON .0125

__kernel
void
ray_trace_bit_scene_opt(__constant  RenderSceneInfo    * linfo,
                        __global    ushort2            * block_ptrs,
                        __global    int4               * tree_array,
                        __global    float              * alpha_array,
                        __global    uchar8             * mixture_array,
                        __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                        __global    uint4              * imgdims,       // dimensions of the image
                        __local     uchar16            * local_tree,
                        __global    float              * in_image,
                        __global    uint               * gl_image, 
                        __constant  uchar              * bit_lookup)    // input image and store vis_inf and pre_inf
{
  //get local id (0-63 for an 8x8) of this patch 
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //----------------------------------------------------------------------------
  // get image coordinates and camera, 
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;  map_work_space_2d(&i,&j);
  int imI = j*get_global_size(0)+i;     //locally store the final index to save a register

  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    gl_image[imI] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[imI] = (float)-1.0f;
    return;
  }
  
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
    gl_image[imI] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[imI] = (float)-1.0f;
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
  float blkCount = 0.0;
  while(tblock < tfar) 
  {
    blkCount++;
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

      //expected_int+=data_ptr;
      step_cell_render_opt2(mixture_array, alpha_array, data_ptr, d, 
                            &vis, &expected_int, &intensity_norm);
      if(d <= 0) break;
    }
#endif
    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit + tblock + EPSILON;
    tblock = texit;
  }

  
#ifdef DEPTH
  data_return.z+=(1-data_return.w)*tfar;
#endif
#ifdef INTENSITY
 expected_int += (1.0-intensity_norm)*1.0f;
#endif
  gl_image[imI] = rgbaFloatToInt((float4) expected_int);
  //gl_image[imI] = rgbaFloatToInt((float4) blkCount/400.0);
  in_image[imI] = expected_int;
}
