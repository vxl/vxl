#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef COMPINDEX

//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  float* expdepth;
  float* expdepthsqr;
  float* probsum;
  float* t;
  float* vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
/*void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs);*/
void cast_ray_render_vis(int,int,float,float,float,float,float,float,
                         __constant RenderSceneInfo*, __global int4*,
                         __local uchar16*, __constant uchar *,__local uchar *,
                         float*, AuxArgs);


void step_cell_compute_index(float depth,
                             float block_len,
                             __global float  * alpha_data,
                             int      data_ptr,
                             float    d,
                             float  * vis,
                             float  * expected_depth,
                             float  * expected_depth_square,
                             float  * probsum,
                             float * t)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float omega=(*vis) * (1.0f - diff_omega);
  (*probsum)+=omega;
  (*vis)    *= diff_omega;
  (*expected_depth)+=depth*omega;
  (*expected_depth_square)+=depth*depth*omega;
  (*t)=depth*block_len;
}

__kernel
void
compute_loc_index(
              __constant  RenderSceneInfo    * linfo,
              __global  float4             * directions,
              __constant  int                * ray_size,
              __global    int4               * tree_array,       // tree structure for each block
              __constant  uchar              * bit_lookup,       // used to get data_index
              __global    float              * alpha_array,
              __constant  float4            * ray_o,
              __constant  float              * max_dist,
              __global    float              * exp_depth_buf,
              __global    float              * vis_buf,
              __global    float              * prob_buf,
              __global    float              * t_infinity_buf,
              __local     uchar16            * local_tree,       // cache current tree into local memory
              __local     uchar              * cumsum
              )
{
    int gid=get_global_id(0);
    //exp_depth_buf[ gid ] = -100.0f;
    //vis_buf[ gid ] = gid+1;
    //prob_buf[ 0 ] = 1000.0f;

    if (gid >= *ray_size)
      return;

    float4 ray_d = directions[ gid ];

    //declare ray
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, *ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    float expdepth    = 0.0f;
    float expdepthsqr = 0.0f;
    float probsum     = prob_buf[ gid ];
    float vis_rec     = vis_buf[ gid ];
    float t           = t_infinity_buf[ gid ];
    float tfar_max = (*max_dist)/linfo->block_len;

    AuxArgs aux_args;
    aux_args.alpha  = alpha_array;
    aux_args.expdepth = &expdepth;
    aux_args.expdepthsqr = &expdepthsqr;
    aux_args.probsum = &probsum;
    aux_args.t = &t;
    aux_args.vis = &vis_rec;

    /*float vis = 1.0;
    cast_ray( 1, 1,
              ray_ox, ray_oy, ray_oz,
              ray_dx, ray_dy, ray_dz,
              linfo, tree_array,                                    //scene info
              local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info*/


    cast_ray_render_vis( 1, 1,
                         ray_ox, ray_oy, ray_oz,
                         ray_dx, ray_dy, ray_dz,
                         linfo, tree_array,                                    //scene info
                         local_tree, bit_lookup, cumsum, &tfar_max, aux_args); //utility info

    //store values at the end of this block
    exp_depth_buf[ gid ] += (* aux_args.expdepth)*linfo->block_len;
    prob_buf[ gid ] = (* aux_args.probsum);
    vis_buf[ gid ]  = vis_rec;
    t_infinity_buf[ gid ]  = (* aux_args.t);
}


__kernel void normalize_index_depth_kernel(
                                         __global    float              * exp_depth_buf,
                                         __global    float              * prob_buf,
                                         __global    float              * t_infinity_buf,
                                         __global    float              * sub_block_dim)
{
    int gid=get_global_id(0);

    //normalize
    float prob   = prob_buf[gid];
    float mean   = exp_depth_buf[gid] + t_infinity_buf[gid]*prob;
    //float mean   =  exp_depth_buf[gid] + t_infinity_buf[gid]*prob * (*sub_block_dim);
    exp_depth_buf[gid]=mean;
}

#endif //COMPINDEX

