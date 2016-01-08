// This is brl/bseg/boxm2/ocl/cl/cone/render_adaptive_cone_kernels.cl
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
//:
// \file
#ifdef MOG_TYPE_16
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_ushort8_sat_rte(data);
    #define MOG_TYPE ushort8
    #define NORM 65535;
#endif
#ifdef MOG_TYPE_8
   #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_uchar8_sat_rte(data);
   #define MOG_TYPE uchar8
   #define NORM 255;
#endif

#ifdef RENDER

//: Define aux_args (like a functor struct)
// \todo Begin passing around AuxArgs* instead of the struct to save on registers
typedef struct
{
  //scene data
  __global float* alphas;
  __global MOG_TYPE* mog;

  //aux data
  __global int* cell_vol;
  __global int* cell_obs;

           //per ball statistics, used in compute ball properties
           float* pi_cum;
           float* vol_cum;
           float* vis_cum;
           float* weighted_int;
           float* intensity_norm;

           //constants used by stepcell functions
           float volume_scale;

  //store ray vis and pre locally
  __local float* vis;
  __local float* expint;

  //curr t, 8x8 matrix
  __local float* currT;

  //store active ray pointer, image/ray pyramids
  __local uchar* active_rays;
  __local uchar* master_threads;
    ray_pyramid* rays;
  image_pyramid* tfar;
} AuxArgs;

void cast_adaptive_cone_ray(
                            //---- RAY ARGUMENTS -------------------------------------------------
                            int i, int j,                                     //pixel information
                            float4 ray_o,                                     //ray origin
                            float4 ray_d,                                     //ray direction + half angle in the w

                            //---- SCENE ARGUMENTS------------------------------------------------
                            __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                            __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

                            //---- UTILITY ARGUMENTS----------------------------------------------
                            __local     uchar16            * local_tree,      //local tree for traversing (8x8 matrix)
                            __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
                            __constant  float              * centerX,         // center points for ...
                            __constant  float              * centerY,         // each of the 585 possible cells ...
                            __constant  float              * centerZ,         // indexed by bit index
                            __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                            __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread

                            //----aux arguments defined by host at compile time-------------------
                            AuxArgs aux_args );

__kernel
void
render_adaptive_cone(__constant  RenderSceneInfo    * linfo,
                     __global    int4               * tree_array,       // tree structure for each block
                     __global    float              * alpha_array,      // alpha for each block
                     __global    MOG_TYPE           * mixture_array,    // mixture for each block
                     __global    float4             * ray_origins,
                     __global    float4             * ray_directions,
                     __global    float              * in_image,         // expected image buffer
                     __global    uint4              * imgdims,          // dimensions of the input image (ROI)
                     __global    float              * output,
                     __constant  uchar              * bit_lookup,       // used to get data_index
                     __global    float              * vis_image,        // visibility image (for keeping vis across blocks)

                     __constant  float              * centerX,          //cached lookup tables for center of cells
                     __constant  float              * centerY,
                     __constant  float              * centerZ,

                     __local     uchar16            * local_tree,       // cache current tree into local memory
                     __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
                     __local     uchar              * to_visit )        //local mem space for BFS on trees
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // get image coordinates and camera, check for validity before proceeding
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  int imIndex = j*get_global_size(0) + i;
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  //----------------------------------------------------------------------------
  // transform rays from world to normalized block world space
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float cone_half_angle = ray_d.w; ray_d.w = 0.0f;
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  //create local memory for ray/image/t pyramids
  //----------------------------------------------------------------------------
  //INITIALIZE RAY PYRAMID
  __local float4* ray_pyramid_mem[4];
  __local float4 ray0[1];
  __local float4 ray1[4];
  __local float4 ray2[16];
  __local float4 ray3[64];
  ray_pyramid_mem[0] = ray0;
  ray_pyramid_mem[1] = ray1;
  ray_pyramid_mem[2] = ray2;
  ray_pyramid_mem[3] = ray3;
  ray3[llid] = (float4) (ray_dx, ray_dy, ray_dz, cone_half_angle);
  barrier(CLK_LOCAL_MEM_FENCE);
  ray_pyramid pyramid = new_ray_pyramid(ray_pyramid_mem, 4, 8);

  //initialize T pyramids (tfar)
  __local float* tfar_mem[4];
  __local float tfar0[1];
  __local float tfar1[4];
  __local float tfar2[16];
  __local float tfar3[64];
  tfar_mem[0] = tfar0;
  tfar_mem[1] = tfar1;
  tfar_mem[2] = tfar2;
  tfar_mem[3] = tfar3;
  tfar3[llid] = 0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid tfar_pyramid = new_image_pyramid(tfar_mem, 4, 8);

  //init active ray matrix
  __local uchar active_rays[64];
  active_rays[llid] = (llid==0) ? 1 : 0;
  barrier(CLK_LOCAL_MEM_FENCE);

  //init master thread matrix
  __local uchar master_threads[64];
  master_threads[llid] = 0; //llid;
  barrier(CLK_LOCAL_MEM_FENCE);

  //init local pre and vis
  __local float vis[64];
  __local float expint[64];
  expint[llid] = 0.0f; //pre_image[imIndex];
  vis[llid] = vis_image[imIndex];
  barrier(CLK_LOCAL_MEM_FENCE);

  //8x8 currT
  __local float currT[64];
  currT[llid] = 0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);

  //store in aux_arg struct
  AuxArgs aux_args;
  aux_args.active_rays = active_rays;
  aux_args.master_threads = master_threads;
  aux_args.tfar  = &tfar_pyramid;
  aux_args.rays  = &pyramid;
  aux_args.vis = vis;
  aux_args.currT = currT;
  aux_args.expint = expint;

  //----------------------------------------------------------------------------
  //store other aux args
  //----------------------------------------------------------------------------
  aux_args.alphas = alpha_array;    //store alpha (gamma) and MOG buffers
  aux_args.mog = mixture_array;

  float pi_cum = 0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  float weighted_int = 0.0f;
  float intensity_norm = 0.0f;
  aux_args.pi_cum = &pi_cum;        //sphere-scope prob(intensity) var
  aux_args.vol_cum = &vol_cum;      //sphere-scope intersected volume var
  aux_args.vis_cum = &vis_cum;      //sphere-scope visibility var
  aux_args.weighted_int = &weighted_int;
  aux_args.intensity_norm = &intensity_norm;
  aux_args.volume_scale = linfo->block_len*linfo->block_len*linfo->block_len; //volume factor

  //----------------------------------------------------------------------------
  // cast adaptive cone ray call
  //----------------------------------------------------------------------------
  cast_adaptive_cone_ray( i, j,
                          (float4) (ray_ox, ray_oy, ray_oz, 0.0f),
                          (float4) (ray_dx, ray_dy, ray_dz, cone_half_angle),
                          linfo, tree_array,                      //scene info
                          local_tree, bit_lookup, centerX, centerY, centerZ,
                          cumsum, to_visit, aux_args);      //utility info

  //store vis/pre/norm
  vis_image[imIndex] = vis[llid]; //vis;

  //store exp int for non active rays
  uchar thread_leader = master_threads[llid];
  in_image[imIndex] = expint[thread_leader];
  vis_image[imIndex] = vis[thread_leader];

}


//----------------------------------------------------------------------------
// Split ray function
//----------------------------------------------------------------------------
void split_ray(AuxArgs aux_args, int side_len)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //turn on the four neighboring threads
  float nextVis = pow(aux_args.vis[llid], 0.25f);
  float nextExp = aux_args.expint[llid];
  for (int ioff=0; ioff<2; ++ioff) {
    for (int joff=0; joff<2; ++joff) {
      //"neighbor" threads are not necessarily neighboring in workspace (only at finest level they are)
      int di = ioff * (side_len/2);
      int dj = joff * (side_len/2);

      //calc local thread ID (in 8x8 workspace)
      uchar id = (localI+di) + (localJ+dj)*get_local_size(0);

      //set the vis and pre for new threads
      aux_args.vis[id] = nextVis;
      aux_args.expint[id] = nextExp;
    } //end i for
  } //end j for
}

//FUNCTORS used with cone ray
void step_cell_cone(AuxArgs aux_args, int data_ptr, float volume)
{
  //scale the volume back to world coordinates
  volume *= aux_args.volume_scale;

  //grab voxel alpha and intensity
  float alpha = aux_args.alphas[data_ptr];

  //calculate the mean intensity
  uchar8 data = aux_args.mog[data_ptr];
  //uchar8 data = (uchar8) 128;
  float w2 = (data.s2 > 0 && data.s5 > 0) ? (float)(255-data.s2-data.s5) : 0.0f;
  float exp_intensity=(float)data.s0 * (float)data.s2 +
                      (float)data.s3 * (float)data.s5 +
                      (float)data.s6 * w2;
  exp_intensity /= (255.0f*255.0f);

  //weighted intensity for this voxel
  (*aux_args.weighted_int) += volume*exp_intensity; //cell_occupancy_prob * volume * exp_intensity;
  (*aux_args.vol_cum) += volume;

  //update vis...
  float temp = exp(-volume*alpha);  //visibility of intersection of this cell and ball
  (*aux_args.vis_cum) *= temp;               //
}

//once step cell is performed, compute ball properties
void compute_ball_properties( AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int ray_level = aux_args.active_rays[llid]-1;
  if(ray_level >= 0) {
    if (*aux_args.vol_cum > 1e-10)
    {
      //calculate ray/sphere occupancy prob = 1-vis_ball
      float sphere_occ_prob = 1.0f - (*aux_args.vis_cum); //(*aux_args.pi_cum) / (*aux_args.vol_cum);

      //calc expected int = weighted sum / weighted total volume
      float expected_int = (*aux_args.weighted_int) / (*aux_args.vol_cum);

      //expected intensity is Visibility * Weighted Intensity * Occupancy
      float ei = aux_args.expint[llid]; //(*aux_args.expint);
      float vis = aux_args.vis[llid];
      ei += vis * expected_int * sphere_occ_prob;
      aux_args.expint[llid] = ei;

      //update visibility after all cells have accounted for
      vis *= (*aux_args.vis_cum);
      aux_args.vis[llid] = vis;
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  *aux_args.vol_cum = 0.0f;
  *aux_args.intensity_norm = 0.0f;
  *aux_args.weighted_int = 0.0f;
  *aux_args.pi_cum = 0.0f;
  *aux_args.vis_cum = 1.0f;
}

#endif // RENDER
