//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef SEGLENVIS

typedef struct
{
  __global float * alpha;
  __global int  * seg_len;
  __global int  * vis_array;
  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
 // __local  float*  cached_vis;
           float*  ray_vis;
           float*  last_vis;
} AuxArgs;


//bayes step cell functor
void step_cell_seglen_vis(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------

    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    //calculate this ray's contribution to beta
    float alpha = aux_args.alpha[data_ptr];
    //float cell_vis = (* aux_args.ray_vis) * d;
    float cell_vis = *(aux_args.last_vis);

    //update ray_pre and ray_vis
    float temp = exp(-alpha * d);
    // updated visibility probability
    (* aux_args.ray_vis) *= temp;

    // keep track of vis at last "empty" cell to prevent self-shadowing within uncertain regions
    constexpr float passthrough_prob_thresh = 0.9;
    if (temp >= passthrough_prob_thresh) {
       *(aux_args.last_vis) = *(aux_args.ray_vis);
    }

    //discretize and store beta and vis contribution
    int vis_int  = convert_int_rte(d * cell_vis * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //--------------------------------------------------------------------------
}
//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
seg_len_and_vis_main(__constant  RenderSceneInfo    * linfo,
                     __global    int4               * tree_array,        // tree structure for each block
                     __global    float              * alpha_array,       // alpha for each block
                     __global    int                * aux_array0,        // four aux arrays strung together
                     __global    int                * aux_array1,        // four aux arrays strung together
                     __constant  uchar              * bit_lookup,        // used to get data_index
                     __global    float4             * ray_origins,
                     __global    float4             * ray_directions,
                     __global    uint4              * imgdims,           // dimensions of the input image
                     __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
                     __global    float              * last_vis_image,    // like visibility image, but updates only in "empty" regions to prevent self-shadowing within uncertain regions
                     __global    float              * output,
                     __local     uchar16            * local_tree,        // cache current tree into local memory
                     __local     short2             * ray_bundle_array,  // gives information for which ray takes over in the workgroup
                     __local     int                * cell_ptrs,         // local list of cell_ptrs (cells that are hit by this workgroup
                     __local     float              * cached_vis,        // cached vis used to sum up vis contribution locally
                     __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
{
  // get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // initialize pre-broken ray information (non broken rays will be re initialized)
  ray_bundle_array[llid] = (short2) (-1, 0);
  cell_ptrs[llid] = -1;

  //----------------------------------------------------------------------------
  // get image coordinates and camera,
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  float vis = vis_image[j*get_global_size(0) + i];
  float last_vis = last_vis_image[j*get_global_size(0) + i];
  float vis0 = 1.0; // don't want to terminate raytrace early, even if visibility is 0

  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
  float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.alpha       = alpha_array;
  aux_args.seg_len     = aux_array0;
  aux_args.vis_array   = aux_array1;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  //aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  aux_args.last_vis = &last_vis;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  last_vis_image[j*get_global_size(0)+i] = last_vis;

}
#endif

#ifdef UPDATE_SUN_VIS
__kernel void
update_visibilities_main(__constant  RenderSceneInfo    * linfo,
                         __global float* aux_array0,
                         __global float* aux_array1,
                         __global float* aux_output)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;
  if (gid<datasize)
  {
    int obs0= as_int(aux_array0[gid]);
    int obs1= as_int(aux_array1[gid]);
    if(obs0>0)
        aux_output[gid] = ((float)obs1)/((float)obs0);
    else
        aux_output[gid] = 0.0;
  }

}
#endif
