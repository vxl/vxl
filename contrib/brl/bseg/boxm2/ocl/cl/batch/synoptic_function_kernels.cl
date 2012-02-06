#ifdef COMPUTE_CUBIC
__kernel
void batch_fit_cubic_polynomial(__global float * aux0,
                                __global float * aux1,
                                __global float * aux2,
								__global float * aux3,
                                __constant int * nobs,
                                __constant float * interim_sigma,
                                __global int *datasize,
                                __global float * cubic_coeffs,
                                __local float * obs,
                                __local float * vis,
								__local float * posts,
                                __local float * s,
                                __local float * temp,
                                __local float * XtWX,      // dim 16
                                __local float * cofactor,  // dim 16
                                __local float * invXtWX,   // dim 16
                                __local float * XtY,       // dim 4
                                __local float * outerprodl,// dim 16
                                __local float * l,         // dim 4
								__local int * num_visible_views
                               )
{
    unsigned gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    if (gid<(*datasize))
    {
        if (llid < (*nobs) )
        {
            // OBTAIN obs, vis
            float seg_len = aux0[(*datasize)*llid+gid];
            obs[llid] = 0.0;
            vis[llid] = 0.0;
			posts[llid] = 0.0;
            if (seg_len > 1e-10f)
            {
                obs[llid]   = aux1[(*datasize)*llid+gid]/seg_len;
                vis[llid]   = aux2[(*datasize)*llid+gid]/seg_len;
				posts[llid] = aux3[(*datasize)*llid+gid]/seg_len;
            }
            // OBTAIN  phi
            seg_len = aux0[(*datasize)*(llid + *nobs)+gid];
            s[llid] = seg_len;//0.0;
            if (seg_len > 1e-10f)
            {
                float x = aux1[(*datasize)*(llid + *nobs)+gid];
                float y = aux2[(*datasize)*(llid + *nobs)+gid];
                s[llid]  = atan2(y,x);
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
		if(llid == 0)
		{
			(*num_visible_views) = 0;
			for(unsigned int i = 0; i < (*nobs); i++)
			{
				if(vis[i] > 0.001)
					(*num_visible_views)=(*num_visible_views) +1 ;
			}
			cubic_coeffs[gid*8+7] =(float) (*num_visible_views);
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		fit_intensity_cubic(obs,vis,s,temp, XtWX, cofactor, invXtWX,  XtY,outerprodl,l, cubic_coeffs, nobs);
		cubic_fit_error(obs, vis,s, temp, cubic_coeffs, interim_sigma, nobs);
		//compute_empty(obs,vis,XtWX,cubic_coeffs,nobs);
		compute_empty(posts,vis,cubic_coeffs,nobs);
		float p1= cubic_coeffs[gid*8+6]/cubic_coeffs[gid*8+5];
		cubic_coeffs[gid*8+7]= min(p1,1/p1);
    }
}
#endif // COMPUTE_CUBIC

#ifdef PREINF_DEPTH_CUBIC
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
           float* vis_inf;
           float* pre_inf;
		   float* pre_depth_inf;
		   float phi;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs);

__kernel
void
pre_depth_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    int                * aux_array0,        // four aux arrays strung together
             __global    int                * aux_array1,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * vis_image,        // visibility image
             __global    float              * pre_inf_image,        // preinf image
             __global    float              * pre_depth_inf_image,        // preinf image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )           // cumulative sum for calculating data pointer
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

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  //float4 inImage = in_image[j*get_global_size(0) + i];
  float vis_inf = vis_image[j*get_global_size(0) + i];
  float pre_inf = pre_inf_image[j*get_global_size(0) + i];
  float pre_depth_inf = pre_depth_inf_image[j*get_global_size(0) + i];

  if (vis_inf <0.0)
    return;
  //vis for cast_ray, never gets decremented so no cutoff occurs
  float vis = 1.0f;
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
  aux_args.linfo   = linfo;
  aux_args.alpha   = alpha_array;
  aux_args.mog     = mixture_array;
  aux_args.seg_len   = aux_array0;
  aux_args.mean_obs  = aux_array1;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  aux_args.pre_depth_inf = &pre_depth_inf;
  aux_args.phi     = atan2(ray_d.y,ray_d.x);
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info

  //store the vis_inf/pre_inf in the image
  vis_image[j*get_global_size(0)+i] = vis_inf;
  pre_inf_image[j*get_global_size(0)+i] = pre_inf;
  pre_depth_inf_image[j*get_global_size(0)+i] = pre_depth_inf;
}
#endif

#ifdef UPDATE_DEPTH_DENSITY
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
  __global int* mean_vis;
  __global int* depth_density;
           float* vis;
		   float* vis_inf;
           float* pre_inf;
		   float* pre_depth_inf;
           float* pre;
		   float* pre_depth;
		   float phi;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs);

__kernel
void
update_post_depth_density(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    int                * aux_array0,        // cum_len
             __global    int                * aux_array1,        // mean_obs
             __global    int                * aux_array2,        // mean vis
             __global    int                * aux_array3,        // mean_density
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * vis_image,        // visibility image
             __global    float              * vis_inf_image,        // visibility image
             __global    float              * pre_inf_image,        // preinf image
             __global    float              * pre_depth_inf_image,        // preinf image
             __global    float              * pre_image,        // preinf image
             __global    float              * pre_depth_image,        // preinf image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )           // cumulative sum for calculating data pointer
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

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  //float4 inImage = in_image[j*get_global_size(0) + i];
  float curr_vis = vis_image[j*get_global_size(0) + i];
  float vis_inf = vis_inf_image[j*get_global_size(0) + i];
  
  float pre = pre_image[j*get_global_size(0) + i];
  float pre_depth = pre_depth_image[j*get_global_size(0) + i];

  float pre_inf = pre_inf_image[j*get_global_size(0) + i];
  float pre_depth_inf = pre_depth_inf_image[j*get_global_size(0) + i];

  //vis for cast_ray, never gets decremented so no cutoff occurs
  float vis = 1.0f;
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
  aux_args.linfo   = linfo;
  aux_args.alpha   = alpha_array;
  aux_args.mog     = mixture_array;
  aux_args.seg_len   = aux_array0;
  aux_args.mean_obs  = aux_array1;
  aux_args.mean_vis  = aux_array2;
  aux_args.depth_density  = aux_array3;
  aux_args.vis = &curr_vis;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  aux_args.pre_depth_inf = &pre_depth_inf;
  aux_args.pre = &pre;
  aux_args.pre_depth = &pre_depth;
  aux_args.phi     = atan2(ray_d.y,ray_d.x);
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info

  //store the vis_inf/pre_inf in the image
  vis_image[j*get_global_size(0)+i] = curr_vis;
  pre_image[j*get_global_size(0)+i] = pre;
  pre_depth_image[j*get_global_size(0)+i] = pre_depth;
}
#endif

#ifdef AVG_SURFACE_EMPTY_RATIO
typedef struct
{
  __global float*   alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
  __global int* vis_array;
  __global int* beta_array;
  
           float   norm;
           float*  ray_vis;
           float*  ray_pre;
           float   pre_inf; 

		   float phi;

  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs);

__kernel
void
avg_surface_empty_ratio_main(__constant  RenderSceneInfo    * linfo,
							 __global    int4               * tree_array,        // tree structure for each block
							 __global    float              * alpha_array,       // alpha for each block
							 __global    MOG_TYPE           * mixture_array,     // mixture for each block
							 __global    int                * aux_array0,        // four aux arrays strung together
							 __global    int                * aux_array1,        // four aux arrays strung together
							 __global    int                * aux_array2,        // four aux arrays strung together
							 __global    int                * aux_array3,        // four aux arrays strung together
							 __constant  uchar              * bit_lookup,        // used to get data_index
							 __global    float4             * ray_origins,
							 __global    float4             * ray_directions,
							 __global    uint4              * imgdims,           // dimensions of the input image
							 __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
							 __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
							 __global    float              * pre_inf_image,        // norm image (for bayes update normalization factor)
							 __global    float              * norm_image,        // norm image (for bayes update normalization factor)
							 __global    float              * output,
							 __local     uchar16            * local_tree,        // cache current tree into local memory
							 __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
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

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  float vis0 = 1.0f;
  float norm = norm_image[j*get_global_size(0) + i];
  float vis = vis_image[j*get_global_size(0) + i];
  float pre = pre_image[j*get_global_size(0) + i];
  float pre_inf = pre_inf_image[j*get_global_size(0) + i];
  if (vis <0.0)
    return;

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
  aux_args.linfo      = linfo;
  aux_args.alpha      = alpha_array;
  aux_args.mog        = mixture_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.beta_array = aux_array3;
  aux_args.phi		  = atan2(ray_d.y,ray_d.x);
  aux_args.norm = norm;
  aux_args.pre_inf = pre_inf;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
  
}
#endif

#ifdef COMPUTE_UNCERTAINTY
__kernel
void compute_uncertainty(__global float * aux0,	
						 __global float * aux2,
						 __global float * aux3,
						 __constant int * nobs,
					     __global int *datasize,
						 __global float * cubic_coeffs,
						 __local float * rs,
						 __local float * viss)
{
	unsigned gid = get_group_id(0);
	uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
	if (gid<(*datasize))
	{
		if (llid < (*nobs) )
		{
			// OBTAIN obs, vis
			float seg_len = aux0[(*datasize)*llid+gid];
			float vis = aux2[(*datasize)*llid+gid];
			float r = aux3[(*datasize)*llid+gid];
			if (seg_len/SEGLEN_FACTOR > 1e-10f)
			{
				rs[llid]   = r/seg_len;
				viss[llid]   = vis/seg_len;
			}
			else
			{
				rs[llid]   = 0.0;
				viss[llid]   = 0.0;
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		if(llid  == 0 )
		{
			float mean_ratios = 0.0f;
			float sum_vis = 0.0f;
			for(unsigned k = 0 ; k < (*nobs); k++)
			{
				if(viss[k]>0.0 && rs[k]>0.0)
				{
					mean_ratios +=log(rs[k])*viss[k];
					sum_vis += viss[k];
				}
			}
			if(sum_vis > 0.0f)
			{
				mean_ratios =  mean_ratios/sum_vis;
				cubic_coeffs[gid*8+7] = exp(mean_ratios);
			}
			else 
			{
				cubic_coeffs[gid*8+7] = 1.0f;
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

}

#endif