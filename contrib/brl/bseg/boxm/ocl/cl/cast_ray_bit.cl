 #pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f

void
cast_ray( 
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction
           
          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    ushort2            * block_ptrs,      //3d array of blk pointers
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
          __global    float              * alpha_array,     //voxel density buffer
          __global    uchar8             * mixture_array,   //appearance model buffer
          __global    uchar              * last_weight_array,//third weight for mixture model
          __global    int                * num_obs_array,   // num obs for each block
          __global    float2             * cum_len_beta,    // cumulative ray length and beta aux vars
          __global    uchar2             * mean_obs_cum_vis,// mean_obs per cell and cumulative visibility


          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      int                  factor,          //factor

          //---- SEPARATE ARGS FOR EACH STEP CELL FUNCTOR... -------------------
#if defined(CHANGE) || defined(RENDER)
          __local     int                * imIndex,         //image index
#endif
#ifdef SEGLEN
          __global    float16            * mat_cam, 
          __local     short2             * ray_bundle_array,//gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,       //local list of cell_ptrs (cells that are hit by this workgroup
                      float                inImgObs,         //input image observation (no need for image_vect... )
                      float                ray_vis, 
          __local     float4             * cached_aux_data,  //local data per ray in workgroup
#endif
#ifdef PREINF  
          __global    float16            * mat_cam, 
                      float4               image_vect,      //input image and store vis_inf and pre_inf
#endif
#ifdef BAYES
          __global    float16            * mat_cam, 
          __local     short2             * ray_bundle_array,//gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,       //local list of cell_ptrs (cells that are hit by this workgroup
                      float                norm,
                      float                ray_vis,
                      float                ray_pre,
#endif 

          //---- OUTPUT ARGUMENTS-----------------------------------------------
          __global    float4             * in_image,        //input image and store vis_inf and pre_inf
          __global    uint               * gl_image,        //gl_image automatically rendered to the screen
          __global    float              * output)          //debug output buffer
{
  
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // pixel values/depth map to be returned
  float vis = 1.0f;
  float expected_int = 0.0f;
#ifdef CHANGE
  float4 temp=in_image[imIndex[llid]];
  float intensity=temp.x;
  float e_intensity=temp.y;
  float change_density_vacuous=0.0f;
  float bmin=1.0f;
  float bmax=0.0f;
  float bsum=0.0f;
  float ray_length=0.0f;
  float e_expected_int=0.0f;
#endif

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = min(min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = max(max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));


  if (tfar <= tblock) {
#ifdef RENDER
    gl_image[imIndex[llid]] = rgbaFloatToInt((float4)(0.0f,0.0f,0.0f,0.0f));
    in_image[imIndex[llid]] = (float)0.0f;
#endif

    return;
  }
  //make sure tnear is at least 0...
  tblock = (tblock > 0.0f) ? tblock : 0.0f;
  
  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar -= BLOCK_EPSILON;   
  
  

  
  //used for depth map 
  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  float cellCount = 0.0f;

  while(tblock < tfar && vis > .01f) 
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
    float cell_minx = clamp(floor(posx), 0.0f, linfo->dims.x-1.0f);
    float cell_miny = clamp(floor(posy), 0.0f, linfo->dims.y-1.0f);
    float cell_minz = clamp(floor(posz), 0.0f, linfo->dims.z-1.0f);

    //load current block/tree 
    int blkIndex = convert_int(cell_minz + (cell_miny + cell_minx*linfo->dims.y)*linfo->dims.z); 
    ushort2 block = block_ptrs[blkIndex];                       
    int root_ptr = block.x * linfo->tree_len + block.y;         
    local_tree[llid] = as_uchar16(tree_array[root_ptr]);        
    
    //initialize cumsum buffer and cumIndex
    cumsum[llid*10] = local_tree[llid].s0;                     
    int cumIndex = 1;                                         
    barrier(CLK_LOCAL_MEM_FENCE);                               

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
    float texit = min(min( (cell_minx-ray_ox)*(1.0f/ray_dx), (cell_miny-ray_oy)*(1.0f/ray_dy)), (cell_minz-ray_oz)*(1.0f/ray_dz));
    if(texit <= tblock) break; //need this check to make sure the ray is progressing
        
    //ttree starts at 0, ttree_exit is t exit value in the tree level (scaled from scene level)
    texit = (texit - tblock - BLOCK_EPSILON);  
    float ttree = 0.0f;                 
    while (ttree < texit)              
    {
      cellCount++;
      // point in tree coordinates
      posx = (lrayx + (ttree + TREE_EPSILON)*ray_dx);  
      posy = (lrayy + (ttree + TREE_EPSILON)*ray_dy);   
      posz = (lrayz + (ttree + TREE_EPSILON)*ray_dz);   
      
      // traverse to leaf cell that contains the entry point, set bounding box
      ////data offset is ushort pointed to by tree + bit offset
      float cell_len;
      int data_ptr = traverse_three(&local_tree[llid], 
                                    posx,posy,posz, 
                                    &cell_minx, &cell_miny, &cell_minz, &cell_len); 
      data_ptr = data_index_opt2(&local_tree[llid], data_ptr, bit_lookup, &cumsum[llid*10], &cumIndex, linfo->data_len);
      data_ptr = (block.x*linfo->data_len) + data_ptr;  
      
      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0.0f) ? cell_minx+cell_len : cell_minx; 
      cell_miny = (ray_dy > 0.0f) ? cell_miny+cell_len : cell_miny; 
      cell_minz = (ray_dz > 0.0f) ? cell_minz+cell_len : cell_minz;
      float t1 = min(min( (cell_minx-lrayx)*(1.0f/ray_dx), (cell_miny-lrayy)*(1.0f/ray_dy)), (cell_minz-lrayz)*(1.0f/ray_dz));
  
      //make sure ray goes through the cell with positive seg length
      if(t1 <= ttree) break;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;


////////////////////////////////////////////////////////////////////////////////
// Step Cell Functor
////////////////////////////////////////////////////////////////////////////////
#ifdef RENDER
      step_cell_render_opt2(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
#endif
#ifdef CHANGE
      //step_cell_change_detection_uchar8(mixture_array,alpha_array,data_ptr,d,&vis,&expected_int,intensity);
      step_cell_change_detection_uchar8_w_expected(mixture_array,alpha_array,data_ptr,d,&vis,&expected_int,&e_expected_int,intensity,e_intensity);
#endif
#ifdef SEGLEN
      //keep track of cells being hit
      cell_ptrs[llid] = data_ptr;
      cached_aux_data[llid] = (float4)0.0f;  //leaders retain the mean obs and the cell length
      barrier(CLK_LOCAL_MEM_FENCE);

      //segment the workgroup
      load_data_mutable_opt(ray_bundle_array,cell_ptrs);
      
      //load aux data into local mem
      float  alpha    = alpha_array[data_ptr];
      
      //kernel (seg len obs 2 keeps a running mean, to be converted to char)
      seg_len_obs_opt2(d, 
                      alpha, 
                      inImgObs,
                      &ray_vis,
                      ray_bundle_array,
                      cached_aux_data);
      
      //set aux data (only one at time)
      if (ray_bundle_array[llid].y==1) 
      {
          int cnt=0;
          while(true)
          {
              if(atom_cmpxchg(&num_obs_array[data_ptr*2],0,1)==0)
              {
                  float2 cl_beta  = cum_len_beta[data_ptr];
                  float2 mean_vis = convert_float2(mean_obs_cum_vis[data_ptr])/255.0f;  
                  cached_aux_data[llid].x=cached_aux_data[llid].x+cl_beta.x;
                  if(cached_aux_data[llid].x>1e-10f)
                  {
                      mean_vis.x=(mean_vis.x*cl_beta.x+cached_aux_data[llid].y)/(cached_aux_data[llid].x);
                      mean_vis.y=(mean_vis.y*cl_beta.x+cached_aux_data[llid].z)/(cached_aux_data[llid].x);
                  }
                  cum_len_beta[data_ptr] = (float2) (cached_aux_data[llid].x, 0.0f);
                  mean_obs_cum_vis[data_ptr] = convert_uchar2_sat_rte(mean_vis*255.0f);
                  atom_xchg(&num_obs_array[data_ptr*2],0);
                  cnt=-1;
              }
              if(cnt=-1)
                  break;
          }
      }
      //reset cell_ptrs to negative one every time (prevents invisible layer bug)
      cell_ptrs[llid] = -1;
#endif
#ifdef PREINF
      
      //keep track of cells being hit
      //cell data, i.e., alpha and app model is needed for some passes 
      float  alpha    = alpha_array[data_ptr];
      float8 mixture  = convert_float8(mixture_array[data_ptr])/255.0f;
      float  weight3  = convert_float(last_weight_array[data_ptr])/255.0f;

      float2 cl_beta  = cum_len_beta[data_ptr];
      float2 mean_vis = convert_float2(mean_obs_cum_vis[data_ptr])/255.0f;

      //calculate pre_infinity denomanator (shape of image)
      pre_infinity_opt(d, cl_beta.x, mean_vis.x, &image_vect, alpha, mixture, weight3);
      
      //aux data doesn't need to be set, just in_image
      in_image[j*get_global_size(0)+i] = image_vect;
#endif
#ifdef BAYES

      //keep track of cells being hit
      cell_ptrs[llid] = data_ptr;
      barrier(CLK_LOCAL_MEM_FENCE);
      load_data_mutable_opt(ray_bundle_array, cell_ptrs);
      
      //if this current thread is a segment leader...
      //      /* cell data, i.e., alpha and app model is needed for some passes */
      float  alpha    = alpha_array[data_ptr];
      float8 mixture  = convert_float8(mixture_array[data_ptr])/255.0f;
      float  weight3  = convert_float(last_weight_array[data_ptr])/255.0f;
      
      //load aux data into local mem
      float2 mean_vis = convert_float2(mean_obs_cum_vis[data_ptr])/255.0f;
      float cell_beta =0;
      
      barrier(CLK_LOCAL_MEM_FENCE);

      //calculate bayes ratio
      bayes_ratio_opt3( d, 
                        mean_vis.x,
                        &ray_pre,
                        &ray_vis,
                        norm, 
                        &cell_beta,
                        ray_bundle_array,
                        cell_ptrs, 
                        alpha, 
                        mixture,
                        weight3);
                        
      //set aux data (only one at time)
      if (ray_bundle_array[llid].y==1) {
          int cnt=0;
          while(true)
          {
              if(atom_cmpxchg(&num_obs_array[data_ptr*2],0,1)==0)
              {
                  float2 cl_beta  = cum_len_beta[data_ptr];
                  cl_beta.y+=cell_beta;
                  cum_len_beta[data_ptr] = cl_beta;
                  atom_xchg(&num_obs_array[data_ptr*2],0);
                  cnt=2000;
              }
              //barrier(CLK_GLOBAL_MEM_FENCE);
              if(cnt=2000)
                  break;
              //cnt++;
          }
          if(cnt==1000)
              output[1]=(float)-66;
      }
      
      //reset cell_ptrs to -1 every time
      cell_ptrs[llid] = -1;
#endif 
////////////////////////////////////////////////////////////////////////////////
// END Step Cell Functor
////////////////////////////////////////////////////////////////////////////////
      
    }

    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit + tblock + BLOCK_EPSILON;
    tblock = texit;
  }

  
#ifdef CHANGE


  expected_int+=vis;
  e_expected_int+=vis;
  float bg_belief=0.0f;
  float fg_belief=0.0f;
  float foreground_density_val=1.0f;
  float pm=e_expected_int/(e_expected_int+1)-0.5*min(e_expected_int,1/e_expected_int);
 
  // Bayesian
  //fg_belief=foreground_density_val*pm/(foreground_density_val*pm+expected_int*pm+2*(1-pm));

  if(expected_int>foreground_density_val)
  { 
      bg_belief=expected_int/(expected_int+foreground_density_val)-foreground_density_val/(2*expected_int);
      fg_belief=0.0;
  }
  else
  {
      bg_belief=0.0;
      fg_belief=(foreground_density_val/(foreground_density_val+expected_int)-expected_int/(2*foreground_density_val))*pm;

  }

  in_image[imIndex[llid]].w=(float)fg_belief;
#endif

#ifdef RENDER
  expected_int += vis*0.5f;

  gl_image[imIndex[llid]] = rgbaFloatToInt((float4) expected_int);
  in_image[imIndex[llid]] = (float4) expected_int;
#endif
}
