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

           //constants used by stepcell functions
           float volume_scale;

  //store ray vis and pre locally
  __local float* vis;
  __local float* pre;

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
  __local float pre[64];
  pre[llid] = 0.0f; //pre_image[imIndex];
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
  aux_args.pre = pre;
  aux_args.currT = currT;

  //----------------------------------------------------------------------------
  //store other aux args
  //----------------------------------------------------------------------------
  aux_args.alphas = alpha_array;    //store alpha (gamma) and MOG buffers
  aux_args.mog = mixture_array;

  float pi_cum = 0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  aux_args.pi_cum = &pi_cum;        //sphere-scope prob(intensity) var
  aux_args.vol_cum = &vol_cum;      //sphere-scope intersected volume var
  aux_args.vis_cum = &vis_cum;      //sphere-scope visibility var
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
}


//----------------------------------------------------------------------------
// Pass one step cell function
//----------------------------------------------------------------------------
void step_cell(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
#if 0
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
  (*aux_args.vol_cum) += intersect_volume;
  (*aux_args.vis_cum) *= temp;
#endif
}

//----------------------------------------------------------------------------
// Pass one compute ball properties function
//----------------------------------------------------------------------------
void compute_ball_properties(AuxArgs aux_args)
{
#if 0
  float vis = (*aux_args.ray_vis); //(*vis_img_)(i,j);
  float pre = (*aux_args.ray_pre); //(*pre_img_)(i,j);
  float PI=0.0;
  if ( *aux_args.vol_cum>1e-12f) PI = (*aux_args.pi_cum) / (*aux_args.vol_cum);

  //incrememnt pre and vis;
  float vis_cum = (*aux_args.vis_cum);
  pre += vis*(1.0-vis_cum)*PI;
  vis *= vis_cum;
  (*aux_args.ray_pre) = pre;
  (*aux_args.ray_vis) = vis;

  //reset ball values
  (*aux_args.vis_cum) = 1.0f;
  (*aux_args.pi_cum) = 0.0f;
  (*aux_args.vol_cum) = 0.0f;
#endif
}

#endif // RENDER
