// This is brl/bseg/boxm2/ocl/cl/cone/update_adaptive_cone_kernels.cl
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

#ifdef PASSONE
//Helper method handles local pyramid declaration
inline image_pyramid declare_local_pyramid(float in_val)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  __local float* obs_mem[4];
  __local float obs0[1];
  __local float obs1[4];
  __local float obs2[16];
  __local float obs3[64];
  obs_mem[0] = obs0;
  obs_mem[1] = obs1;
  obs_mem[2] = obs2;
  obs_mem[3] = obs3;
  obs3[llid] = in_val;
  barrier(CLK_LOCAL_MEM_FENCE);
  return new_image_pyramid(obs_mem, 4, 8);
}

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

           //constants used by stepcell functions
           float obs;
           float volume_scale;

  //store ray vis and pre locally
  __local float* vis;
  __local float* pre;
  image_pyramid* pre_pyramid;

  //curr t, 8x8 matrix
  __local float* currT;

  //store active ray pointer, image/ray pyramids
  __local uchar* active_rays;
  __local uchar* master_threads;

  //multi res ray, image and tfar pyramids
    ray_pyramid* rays;
  image_pyramid* image;
  image_pyramid* tfar;

  //debug value
  __local float* single;
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
pass_one(__constant  RenderSceneInfo    * linfo,
         __global    int4               * tree_array,       // tree structure for each block
         __global    float              * alpha_array,      // alpha for each block
         __global    MOG_TYPE           * mixture_array,    // mixture for each block
         __global    int                * aux_volume,        // seg len aux array
         __global    int                * aux_mean_obs,      // mean obs r aux array
         __constant  uchar              * bit_lookup,       // used to get data_index
         __global    float4             * ray_origins,
         __global    float4             * ray_directions,
         __global    uint4              * imgdims,          // dimensions of the input image
         __global    float              * in_image,         // the input image
         __global    float              * vis_image,        // visibility image (for keeping vis across blocks)
         __global    float              * pre_image,        // preinf image (for keeping pre across blocks)
         __global    float              * norm_image,        // norm image (for bayes update normalization factor)
         __global    float              * output,

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

  //INITIALIZE OBSERVED PYRAMID
  __local float* obs_mem[4];
  __local float obs0[1];
  __local float obs1[4];
  __local float obs2[16];
  __local float obs3[64];
  obs_mem[0] = obs0;
  obs_mem[1] = obs1;
  obs_mem[2] = obs2;
  obs_mem[3] = obs3;
  obs3[llid] = in_image[imIndex];
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid obs_pyramid  = new_image_pyramid(obs_mem, 4, 8);

  //keep multi res pre pyramid...
  __local float* pre_mem[4];
  __local float pre0[1];
  __local float pre1[4];
  __local float pre2[16];
  __local float pre3[64];
  pre_mem[0] = pre0;
  pre_mem[1] = pre1;
  pre_mem[2] = pre2;
  pre_mem[3] = pre3;
  pre3[llid] = 0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid pre_pyramid  = new_image_pyramid(pre_mem, 4, 8);

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
  __local float pre[64];
  pre[llid] = pre_image[imIndex];
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
  aux_args.image = &obs_pyramid;
  aux_args.rays  = &pyramid;
  aux_args.vis = vis;
  aux_args.pre = pre;
  aux_args.pre_pyramid = &pre_pyramid;
  aux_args.currT = currT;

  //----------------------------------------------------------------------------
  //store other aux args
  //----------------------------------------------------------------------------
  aux_args.alphas = alpha_array;    //store data buffers (and aux buffers)
  aux_args.mog = mixture_array;
  aux_args.cell_vol = aux_volume;
  aux_args.cell_obs = aux_mean_obs; //&aux_array[linfo->num_buffer * linfo->data_len];

  float4 obs = in_image[imIndex];
  float norm = norm_image[imIndex];
  float pi_cum = 0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  aux_args.pi_cum = &pi_cum;        //sphere-scope prob(intensity) var
  aux_args.vol_cum = &vol_cum;      //sphere-scope intersected volume var
  aux_args.vis_cum = &vis_cum;      //sphere-scope visibility var
  aux_args.obs = in_image[imIndex]; //observed intensity in image
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

  //store pre value for all
  uchar thread_leader = master_threads[llid];
  pre_image[imIndex] = pre[thread_leader];
  vis_image[imIndex] = vis[thread_leader];

  //store norm image
  norm_image[imIndex] = vis[thread_leader] + pre[thread_leader];
}


//----------------------------------------------------------------------------
// Split ray function
//----------------------------------------------------------------------------
void split_ray(AuxArgs aux_args, int side_len)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);
  int next_level = aux_args.active_rays[llid]-1;

  //turn on the four neighboring threads
  float nextVis = pow(aux_args.vis[llid], 0.25f);
  float nextPre = image_pyramid_access_safe(aux_args.pre_pyramid, next_level); //aux_args.pre[llid];
  for (int ioff=0; ioff<2; ++ioff) {
    for (int joff=0; joff<2; ++joff) {

      //"neighbor" threads are not necessarily neighboring in workspace (only at finest level they are)
      int di = ioff * (side_len/2);
      int dj = joff * (side_len/2);

      //calc local thread ID (in 8x8 workspace)
      uchar id = (localI+di) + (localJ+dj)*get_local_size(0);

      //set the vis and pre for new threads
      aux_args.vis[id] = nextVis;
      aux_args.pre[id] = nextPre;

      //pre contribution for all resolutions finer than this one
      for (int curr_level=next_level+1; curr_level < 4; ++curr_level)
      {
        float val = image_pyramid_access_safe(aux_args.pre_pyramid, curr_level);
        image_pyramid_set_level(aux_args.pre_pyramid, curr_level, localI+di, localJ+dj, val);
      }

    } //end i for
  } //end j for
}

//----------------------------------------------------------------------------
// Pass one step cell function
//----------------------------------------------------------------------------
void step_cell(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  //make sure intersect volume reflects real world scale
  intersect_volume *= aux_args.volume_scale;

  //increment cell volume and cell mean obs, used later
  int vol_int = convert_int_rte(intersect_volume * SEGLEN_FACTOR);
  atom_add( &aux_args.cell_vol[data_ptr], vol_int );
  int cum_obs = convert_int_rte(intersect_volume * aux_args.obs * SEGLEN_FACTOR);
  atom_add( &aux_args.cell_obs[data_ptr], cum_obs );

  // if total length of rays is too small, do nothing
  float8 mixture = convert_float8(aux_args.mog[data_ptr]) / NORM;
  float PI = gauss_3_mixture_prob_density( aux_args.obs,
                                           mixture.s0,
                                           mixture.s1,
                                           mixture.s2,
                                           mixture.s3,
                                           mixture.s4,
                                           mixture.s5,
                                           mixture.s6,
                                           mixture.s7,
                                           (1.0f-mixture.s2-mixture.s5)
                                          );
  float gamma = aux_args.alphas[data_ptr];
  float temp = exp(-intersect_volume*gamma);  //visibility of intersection of this cell and ball

  //accumulate ball statistics
  (*aux_args.pi_cum) += PI*intersect_volume;
  (*aux_args.vol_cum) += intersect_volume;   //total ball volume intersected
  (*aux_args.vis_cum) *= temp;               //
}

//----------------------------------------------------------------------------
// Pass one compute ball properties function
//----------------------------------------------------------------------------
void compute_ball_properties(AuxArgs aux_args)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int ray_level = aux_args.active_rays[llid]-1;
  if (ray_level >= 0) {
    float vis = aux_args.vis[llid]; //(*aux_args.ray_vis); //(*vis_img_)(i,j);
    float pre = aux_args.pre[llid]; //(*aux_args.ray_pre); //(*pre_img_)(i,j);

    float PI=0.0;
    if ( *aux_args.vol_cum>1e-12f)
      PI = (*aux_args.pi_cum) / (*aux_args.vol_cum);

    //incrememnt pre and vis;
    float vis_cum = (*aux_args.vis_cum);  //1-vis_cum = prob(ball \in surface)

    //pre contribution for all resolutions finer than this one
    float pow_val = .25f;
    for (int curr_level=ray_level+1; curr_level < 4; ++curr_level, pow_val*=.25f)
    {
      float vis_level = pow(vis, pow_val);
      float vis_cum_level = pow(vis_cum, pow_val);
      float pre_contr = vis_level*(1-vis_cum_level)*PI;
      image_pyramid_incr_safe(aux_args.pre_pyramid, curr_level, pre_contr);
    }

    //update vis after the fact
    pre += vis*(1.0-vis_cum)*PI;
    vis *= vis_cum;
    aux_args.vis[llid] = vis;
    aux_args.pre[llid] = pre;
  }

  //reset ball values
  (*aux_args.vis_cum) = 1.0f;
  (*aux_args.pi_cum) = 0.0f;
  (*aux_args.vol_cum) = 0.0f;
}
#endif


#ifdef BAYES
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
  __global int* cell_vis;
  __global int* cell_beta;;

           //per ball statistics, used in compute ball properties
           float* pi_cum;
           float* vol_cum;
           float* vis_cum;
           float* beta_cum;

           //constants used by stepcell functions
           float volume_scale;

  //store ray vis and pre locally
  __local float* vis;
  __local float* pre;
  image_pyramid* pre_pyramid;

  //curr t, 8x8 matrix
  __local float* currT;

  //current obs - this will be reset when split
          float norm;

  //store active ray pointer, image/ray pyramids
  __local uchar* active_rays;
  __local uchar* master_threads;

  //multi res ray, image and tfar pyramids
    ray_pyramid* rays;
  image_pyramid* image;
  image_pyramid* tfar;

  //debug value
  __local float* single;
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
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    MOG_TYPE           * mixture_array,    // mixture for each block
           __global    int                * aux_vol,        // seg len aux array
           __global    int                * aux_obs,      // mean obs r aux array
           __global    int                * aux_vis,      // mean obs r aux array
           __global    int                * aux_beta,      // mean obs r aux array
           __constant  uchar              * bit_lookup,       // used to get data_index
           __global    float4             * ray_origins,
           __global    float4             * ray_directions,
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    float              * in_image,         // input image
           __global    float              * vis_image,        // visibility image (for keeping vis across blocks)
           __global    float              * pre_image,        // preinf image (for keeping pre across blocks)
           __global    float              * norm_image,        // norm image (for bayes update normalization factor)
           __global    float              * output,

           __constant  float              * centerX,          //cached lookup tables for center of cells
           __constant  float              * centerY,
           __constant  float              * centerZ,
           __local     uchar16            * local_tree,       // cache current tree into local memory
           __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
           __local     uchar              * to_visit )        //local mem space for BFS on trees
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //----------------------------------------------------------------------------
  // get image coordinates and camera,
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  int imIndex = j*get_global_size(0) + i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  float norm = norm_image[j*get_global_size(0) + i];
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
  float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
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

  //INITIALIZE OBSERVED PYRAMID
  __local float* obs_mem[4];
  __local float obs0[1];
  __local float obs1[4];
  __local float obs2[16];
  __local float obs3[64];
  obs_mem[0] = obs0;
  obs_mem[1] = obs1;
  obs_mem[2] = obs2;
  obs_mem[3] = obs3;
  obs3[llid] = in_image[imIndex];
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid obs_pyramid  = new_image_pyramid(obs_mem, 4, 8);

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

  //keep multi res pre pyramid...
  __local float* pre_mem[4];
  __local float pre0[1];
  __local float pre1[4];
  __local float pre2[16];
  __local float pre3[64];
  pre_mem[0] = pre0;
  pre_mem[1] = pre1;
  pre_mem[2] = pre2;
  pre_mem[3] = pre3;
  pre3[llid] = 0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid pre_pyramid  = new_image_pyramid(pre_mem, 4, 8);

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
  __local float pre[64];
  pre[llid] = pre_image[imIndex];
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
  aux_args.image = &obs_pyramid;
  aux_args.rays  = &pyramid;
  aux_args.vis = vis;
  aux_args.pre = pre;
  aux_args.pre_pyramid = &pre_pyramid;
  aux_args.currT = currT;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  aux_args.alphas   = alpha_array;
  aux_args.mog     = mixture_array;

  //global aux
  aux_args.cell_vol = aux_vol;
  aux_args.cell_obs  = aux_obs; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.cell_vis  = aux_vis; //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.cell_beta = aux_beta; //&aux_array[3 * linfo->num_buffer * linfo->data_len];
  aux_args.norm = norm;
  float pi_cum=0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  float beta_cum = 1.0f;
  aux_args.pi_cum = &pi_cum;
  aux_args.vol_cum = &vol_cum;
  aux_args.vis_cum = &vis_cum;
  aux_args.beta_cum = &beta_cum;
  aux_args.volume_scale = linfo->block_len*linfo->block_len*linfo->block_len;

  //----------------------------------------------------------------------------
  // cast adaptive cone ray call
  //----------------------------------------------------------------------------
  cast_adaptive_cone_ray( i, j,
                          (float4) (ray_ox, ray_oy, ray_oz, 0.0f),
                          (float4) (ray_dx, ray_dy, ray_dz, cone_half_angle),
                          linfo, tree_array,                      //scene info
                          local_tree, bit_lookup, centerX, centerY, centerZ,
                          cumsum, to_visit, aux_args);      //utility info

  //write out vis and pre
  vis_image[imIndex] = vis[llid];

  //store exp int for non active rays
  uchar thread_leader = master_threads[llid];
  pre_image[imIndex] = pre[thread_leader];
}


//----------------------------------------------------------------------------
// Split ray function
//----------------------------------------------------------------------------
void split_ray(AuxArgs aux_args, int side_len)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);
  int next_level = aux_args.active_rays[llid]-1;

  //turn on the four neighboring threads
  float nextVis = pow(aux_args.vis[llid], 0.25f);
  float nextPre = image_pyramid_access_safe(aux_args.pre_pyramid, next_level); //aux_args.pre[llid];
  for (int ioff=0; ioff<2; ++ioff) {
    for (int joff=0; joff<2; ++joff) {

      //"neighbor" threads are not necessarily neighboring in workspace (only at finest level they are)
      int di = ioff * (side_len/2);
      int dj = joff * (side_len/2);

      //calc local thread ID (in 8x8 workspace)
      uchar id = (localI+di) + (localJ+dj)*get_local_size(0);

      //set the vis and pre for new threads
      aux_args.vis[id] = nextVis;
      aux_args.pre[id] = nextPre;

      //pre contribution for all resolutions finer than this one
      for (int curr_level=next_level+1; curr_level < 4; ++curr_level)
      {
        float val = image_pyramid_access_safe(aux_args.pre_pyramid, curr_level);
        image_pyramid_set_level(aux_args.pre_pyramid, curr_level, localI+di, localJ+dj, val);
      }

    } //end i for
  } //end j for
}


bool step_cell(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //rescale intersect volume
  intersect_volume *= aux_args.volume_scale;

  //be sure to grab the correct obs (at the correct level)
  float obs = image_pyramid_access_safe(aux_args.image, aux_args.active_rays[llid]-1);

  //rescale aux args, calculate mean obs
  float8 mixture = convert_float8(aux_args.mog[data_ptr]) / NORM;
  float PI = gauss_3_mixture_prob_density( obs,
                                           mixture.s0,
                                           mixture.s1,
                                           mixture.s2,
                                           mixture.s3,
                                           mixture.s4,
                                           mixture.s5,
                                           mixture.s6,
                                           mixture.s7,
                                           (1.0f-mixture.s2-mixture.s5)
                                          );
  float vis = aux_args.vis[llid]; //(*aux_args.ray_vis);
  float gamma = aux_args.alphas[data_ptr];

  //sum cumulative vis
  int vis_int = convert_int_rte( vis*intersect_volume*SEGLEN_FACTOR );
  atom_add( &aux_args.cell_vis[data_ptr], vis_int );

  //update ball properties
  float temp=exp(-intersect_volume*gamma);
  *aux_args.pi_cum += PI*intersect_volume;
  *aux_args.vol_cum += intersect_volume;
  *aux_args.vis_cum *= temp;
  return true;
}

bool compute_ball_properties(AuxArgs aux_args)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int ray_level = aux_args.active_rays[llid]-1;
  if (ray_level >= 0 ) {
    float vis = aux_args.vis[llid]; //(*aux_args.ray_vis);
    float pre = aux_args.pre[llid]; //(*aux_args.ray_pre);

    float PI=0.0;
    if ( *aux_args.vol_cum>1e-12f )
      PI = (*aux_args.pi_cum) / (*aux_args.vol_cum);

    //incrememnt beta pre and vis along the ray
    (*aux_args.beta_cum) = (pre+vis*PI)/aux_args.norm;  //update current beta value w/ pre/vis
    float vis_cum = (*aux_args.vis_cum);  //1-vis_cum = prob(ball \in surface)

    //pre contribution for all resolutions finer than this one
    float pow_val = .25f;
    for (int curr_level=ray_level+1; curr_level < 4; ++curr_level, pow_val*=.25f)
    {
      float vis_level = pow(vis, pow_val);
      float vis_cum_level = pow(vis_cum, pow_val);
      float pre_contr = vis_level*(1-vis_cum_level)*PI;
      image_pyramid_incr_safe(aux_args.pre_pyramid, curr_level, pre_contr);
    }

    //be sure to update vis after the fact
    pre += vis*(1.0f-vis_cum )*PI;
    vis *= vis_cum;
    aux_args.pre[llid] = pre; //(*aux_args.ray_pre) = pre;
    aux_args.vis[llid] = vis; //(*aux_args.ray_vis) = vis;
  }

  //reset ball values
  (*aux_args.vis_cum) = 1.0f;
  (*aux_args.pi_cum) = 0.0f;
  (*aux_args.vol_cum) = 0.0f;
  return true;
}

bool redistribute(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  intersect_volume *= aux_args.volume_scale;
  int beta_int = convert_int_rte( intersect_volume * (*aux_args.beta_cum) * SEGLEN_FACTOR );
  atom_add( &aux_args.cell_beta[data_ptr], beta_int );
  return true;
}

#endif

__kernel
void
update_cone_data( __global RenderSceneInfo  * info,
                  __global float            * alpha_array,
                  __global MOG_TYPE         * mixture_array,
                  __global ushort4          * nobs_array,
                  __global int              * aux_vol,        // seg len aux array
                  __global int              * aux_obs,      // mean obs r aux array
                  __global int              * aux_vis,      // mean obs r aux array
                  __global int              * aux_beta)     // mean obs r aux array
{
  float  cell_min = info->block_len/(float)(1<<info->root_level);
  float  alphamin = -log(1.0-0.00001) / (cell_min*cell_min*cell_min);
  float t_match = 2.5f;
  float init_sigma = 0.09f;
  float min_sigma = 0.03f;
  int gid=get_global_id(0);
  int datasize = info->data_len * info->num_buffer;
  if (gid<datasize)
  {
    float cell_vol = convert_float(aux_vol[gid]); // / SEGLEN_FACTOR;
    if (cell_vol/SEGLEN_FACTOR>1e-10f)
    {
      float beta = convert_float(aux_beta[gid])/cell_vol;  //(cell_vol*SEGLEN_FACTOR);
      float vis  = convert_float(aux_vis[gid])/cell_vol; //(cell_vol*SEGLEN_FACTOR);
      float mean_obs = convert_float(aux_obs[gid])/cell_vol; //(cell_vol*SEGLEN_FACTOR);

      float alpha = alpha_array[gid];
      MOG_TYPE mog_bytes = mixture_array[gid];
      float8 mog = convert_float8(mog_bytes) / (float) NORM;
      ushort4 num_obs = nobs_array[gid];

      //update alpha
      alpha = alpha*beta;

      //update gauss 3 mixture with mean-obs
      float mu0 = mog.s0, sigma0 = mog.s1, w0 = mog.s2;
      float mu1 = mog.s3, sigma1 = mog.s4, w1 = mog.s5;
      float mu2 = mog.s6, sigma2 = mog.s7;
      float w2=0.0f;
      if (w0>0.0f && w1>0.0f)
        w2=1.0f-w0-w1;

      short Nobs0 = (short) num_obs.s0,
            Nobs1 = (short) num_obs.s1,
            Nobs2 = (short) num_obs.s2;
      float Nobs_mix = num_obs.s3/100.0f;
      update_gauss_3_mixture( mean_obs, vis, t_match, init_sigma, min_sigma,
                              &mu0,&sigma0,&w0,&Nobs0,
                              &mu1,&sigma1,&w1,&Nobs1,
                              &mu2,&sigma2,&w2,&Nobs2,
                              &Nobs_mix );

      //reset the cells in memory
      alpha_array[gid]      = max(alphamin, alpha);
      float8 post_mix       = (float8) (mu0, sigma0, w0,
                                        mu1, sigma1, w1,
                                        mu2, sigma2) * (float) NORM;
      CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix)
      float4 post_nobs      = (float4) (Nobs0, Nobs1, Nobs2, Nobs_mix*100.0);
      nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);
    }

    //zero out all aux data
    aux_vol[gid] = 0;
    aux_obs[gid] = 0;
    aux_vis[gid] = 0;
    aux_beta[gid] = 0;
  }
}

