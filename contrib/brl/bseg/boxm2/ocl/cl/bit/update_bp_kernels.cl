//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef PRE_CELL
// The AuxArgs struct is defined for each step cell case in order to pass custom arguments from the cast_ray function
// note that only one instance of the struct definition can exist in the kernel source code prior to the implementation
// of the cast_ray function. This requirement translates to defining only one cell symbol, e.g. PRE_CELL, for a given process
typedef struct
{
    __global float* alpha;
    __global float * seg_len;
    __global float * datadensity;
	__global float * pre;
	__global float * vis;
    float* vis_inf;
    float* pre_inf;
    __constant RenderSceneInfo * linfo;

} AuxArgs;

//preinf step cell functor
// this function is called on each step along a ray as voxels on the ray are traversed
//=================================================================================
// inputs:                                                                         |
// aux_args - a struct containing custom data for a given step cell implementation |
// data_ptr - a pointer into any data array defined for the cell                   |
// llid     - the local index of the thread processing this ray                    |
// d        - the length of the ray segment intersecting this cell in local units  |
//=================================================================================
void step_cell_pre(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    ////cell data, i.e., alpha and app model is needed for some passes
  float  alpha = aux_args.alpha[data_ptr];         // surface probability is given by p = (1-exp(-alpha*seg_len))
  float seg_len = d*aux_args.linfo->block_len;     // the length of the ray's intersection with the cell in global units (e.g meters)
  float PI = aux_args.datadensity[data_ptr];       // the probability density of the observation, e.g. image intensity

  /* Calculate pre and vis infinity */
  float diff_omega = exp(-alpha * seg_len);
  float vis_prob_end = (*aux_args.vis_inf) * diff_omega; // vis_prob_end = vis_inf * (1-p)
                                                         // = vis_inf * (1-(1-diff_omega)) = vis_inf*diff_omega

  /* updated pre                      Omega         *   PI  */
  (*aux_args.pre_inf) += ((*aux_args.vis_inf) - vis_prob_end) *  PI; // vis_inf - vis_prob_end =
                                                                     // vis_inf - vis_inf(1-p) = 
                                                                     // vis_inf * p

  /* updated visibility probability */
  (*aux_args.vis_inf) = vis_prob_end;
  AtomicAdd((__global float*) (&aux_args.seg_len[data_ptr]), d);
  AtomicAdd((__global float*) (&aux_args.pre[data_ptr]), (*aux_args.pre_inf)*d );
  AtomicAdd((__global float*) (&aux_args.vis[data_ptr]), (*aux_args.vis_inf)*d );
}

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel void
pre_cell_main(__constant  RenderSceneInfo    * linfo,
			 __global    int4               * tree_array,       // tree structure for each block
			 __global    float              * alpha_array,      // alpha for each block
			 __global    float              * aux_array0,        // seg_len
			 __global    float              * aux_array1,        // pre
			 __global    float              * aux_array2,        // vis
             __global    float              * datadensity,        // datadensity
			 __constant  uchar              * bit_lookup,       // used to get data_index
			 __global    float4             * ray_origins,
			 __global    float4             * ray_directions,
			 __global    float              * nearfarplanes,
			 __global    uint4              * imgdims,          // dimensions of the input image
			 __global    float              * vis_image,        // visibility image
			 __global    float              * pre_image,        // preinf image
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
    float pre_inf = pre_image[j*get_global_size(0) + i];

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
    aux_args.seg_len = aux_array0;
    aux_args.pre     = aux_array1;
    aux_args.vis     = aux_array2;
    aux_args.datadensity = datadensity;
    aux_args.vis_inf = &vis_inf;
    aux_args.pre_inf = &pre_inf;

    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    //store the vis_inf/pre_inf in the image
    vis_image[j*get_global_size(0)+i] = vis_inf;
    pre_image[j*get_global_size(0)+i] = pre_inf;
}

__kernel
void normalize_pre_cell(__global RenderSceneInfo  * info,
                        __global float              * aux_array0,       //cum_len
                        __global float              * aux_array1,       // pre term
                        __global float              * aux_array2)       // vis term
{
    int gid = get_global_id(0);
    int datasize = info->data_len;
    if (gid < datasize)
    {
        float cum_len = aux_array0[gid];
        if (cum_len > 1e-7)
        {
            aux_array1[gid] = aux_array1[gid] / cum_len;
            aux_array2[gid] = aux_array2[gid] / cum_len;
        }
        else
        {
            aux_array1[gid] =0.0f;
            aux_array2[gid] =0.0f;
        }
    }
}


#endif // PREINF

#ifdef POST_CELL

typedef struct
{
  __global float* alpha;
  __global float * seg_len;
  __global float * post;
  __global float * datadensity;
  float* vis_inf;
  float* post_inf;
  __constant RenderSceneInfo * linfo;

} AuxArgs;
//post step cell functor
//
// It is necessary to reverse trace the ray in order to incrementally compute
// post since to compute the marginalized probability it is necessary to
// have a running sum based on the previously computed cell information
// and post is, by definition, after the current cell not before.
// Thus, the computation starts at the last cell on the ray intersecting the model
//          c0                            ci                                  cN-1                              cN
//     ]|========|====| ...|================================|...|================================|================================[
//                          post(i)= post(i-1)*(1-p) + p*PI       post(N-1)= post(N)*(1-p) + p*PI           post(N)=p*PI
//                                                                                     ^          don't know vis yet. will be 1-p
//                                            vis i-1                                vis N        of the next cell.
// This diagram should be read right to left
//
void step_cell_post(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha is need for this pass
    float alpha = aux_args.alpha[data_ptr];
    float seg_len = d*aux_args.linfo->block_len;
    float PI = aux_args.datadensity[data_ptr];
    /* Calculate pre and vis infinity */
    float diff_omega = exp(-alpha * seg_len);
    AtomicAdd((__global float*) (&aux_args.post[data_ptr]), (*aux_args.post_inf)*d );// store post for this cell
    AtomicAdd((__global float*) (&aux_args.seg_len[data_ptr]), d);                   // store seg length

    // update post
    /* updated post      =          post       *  (1-p)    +        P        *  PI    */
    (*aux_args.post_inf) = (*aux_args.post_inf)*diff_omega + (1 - diff_omega)*  PI;
}

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel void
post_cell_main(__constant  RenderSceneInfo    * linfo,
			 __global    int4               * tree_array,       // tree structure for each block
			 __global    float              * alpha_array,      // alpha for each block
			 __global    float              * aux_array0,        // four aux arrays strung together
			 __global    float              * aux_array1,        // four aux arrays strung together
			 __global    float              * datadensity,        // four aux arrays strung together
			 __constant  uchar              * bit_lookup,       // used to get data_index
			 __global    float4             * ray_origins,
			 __global    float4             * ray_directions,
			 __global    float              * nearfarplanes,
			 __global    uint4              * imgdims,          // dimensions of the input image
			 __global    float              * vis_image,        // visibility image
			 __global    float              * post_image,        // preinf image
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
    float post_inf = post_image[j*get_global_size(0) + i];

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
    aux_args.linfo     = linfo;
    aux_args.alpha     = alpha_array;
    aux_args.seg_len   = aux_array0;
	aux_args.post	   = aux_array1;
    aux_args.datadensity = datadensity;
    aux_args.vis_inf = &vis_inf;
    aux_args.post_inf = &post_inf;

    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    //store the vis_inf/pre_inf in the image
    vis_image[j*get_global_size(0)+i] = vis_inf;
    post_image[j*get_global_size(0)+i] = post_inf;
}
#endif // POST_CELL
#ifdef NORMALIZE_POST_CELL
__kernel
void normalize_post_cell(__global RenderSceneInfo  * info,
                         __global float              * aux_array0,  // seg_len
                         __global float              * aux_array1)  // post term
{
    int gid = get_global_id(0);
    int datasize = info->data_len;
    if (gid < datasize)
    {
        float cum_len = aux_array0[gid];
        if (cum_len > 1e-7)
            aux_array1[gid] = aux_array1[gid] / cum_len;
        else
            aux_array1[gid] = 0.0f;
    }
}

#endif // NORMALIZE_POST_CELL

#ifdef ADD_SUBTRACT_FACTOR
// Update each cell using its aux data
//
//
// This function is for computing the factor for each viewpoint and is added/subtracted to the factor accumulator. Each factor should look like
// - log( (pre+pdata*vis)/(pre+post*vis)
__kernel
void add_subtract_factor_main(__constant RenderSceneInfo    * info,
				   __constant int * sum,                    // variable for indiicating addition (1) or subtraction(0)
				   __global float              * aux2_pre, // pre
				   __global float              * aux3_pre, // VIS
				   __global float              * aux2_post, //post
				   __global float              * aux3_post, // Pdata
				   __global float			   * aux0_Z)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
	//: todo make this robust
    if (gid<datasize)
    {
      if(aux3_pre[gid] > 1e-10 )
        {
          float pigs  = aux2_pre[gid] + aux3_pre[gid]* aux3_post[gid];
          float pige  = aux2_pre[gid] + aux3_pre[gid]* aux2_post[gid];
          if (pige > 0.0)
        {
          float small = 1e-10;
          float u = max(small, (pigs / pige));
          if ( sum[0] == 1)
            aux0_Z[gid] = aux0_Z[gid] + log(u);
          if ( sum[0] == 0)
            aux0_Z[gid] = aux0_Z[gid] - log(u);
        }

        }
    }
}

#endif // ADD_SUBTRACT_FACTOR

#ifdef INIT_CUM
// initialize each cell with the factor using prior probability
//
__kernel
void init_cum_main(__global RenderSceneInfo    * info,          // scene info
                   __global float * aux0_prob_init,             // array storing intial probability of the voxels
				   __global float			   * aux0_cum)      // array accumualting all facotrs for voxels
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    float pinit = aux0_prob_init[gid];
	//: todo make this robust
    if (gid<datasize)
    {
		if(pinit > 0.0)
                  aux0_cum[gid] = log(pinit/(1-pinit));
    }
}

#endif // init_cum_main

#ifdef INIT_UNIFORM_PROB
//: this function computes intial probability such that P(X)*Vis(X) = 1/(Z_voxels)  along a vertical direction
typedef struct
{
    __global float * alpha_init;
    float * vis;
} AuxArgs;

void step_cell_init_prob(AuxArgs aux_args, int data_ptr, float blk_height, float d)
{
    //keep track of cells being hit
    float psurf = (d / blk_height) / (*aux_args.vis);
    aux_args.alpha_init[data_ptr] = min(psurf,0.99999f);
    (*aux_args.vis) = (*aux_args.vis)* (1 - psurf);
}

//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float, __constant RenderSceneInfo*,
    __global int4*, local uchar16*, constant uchar *, local uchar *, float*, AuxArgs, float tnear, float tfar);

__kernel
void init_prob_main(__constant  RenderSceneInfo    * linfo,            // scene info
                    __global    int4               * tree_array,       // tree structure for each block
                    __global    float              * aux0_alpha_init,  // output array to store intial probability of the voxels
                    __global    float4             * ray_origins,
                    __global    float4             * ray_directions,
                    __global    uint4              * imgdims,          // dimensions of the input image
                    __global    float              * output,
                    __constant  uchar              * bit_lookup,       // used to get data_index
                    __local     uchar16            * local_tree,       // cache current tree into local memory
                    __local     uchar              * cumsum)           // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*imgdims).z || j >= (*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    //vis for cast_ray, never gets decremented so no cutoff occurs
    float vis = 1.0f;
    barrier(CLK_LOCAL_MEM_FENCE);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[j*get_global_size(0) + i];
    float4 ray_d = ray_directions[j*get_global_size(0) + i];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.alpha_init = aux0_alpha_init;
    aux_args.vis = &vis;
    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, 0.0f, MAXFLOAT);    //utility info
}
#endif

#ifdef FUSE_FACTORS
// function to add different factor types to this term
//
__kernel
void fuse_factors_main(__global RenderSceneInfo    * info,
                       __global float			   * aux0_factor,       // array stoing the cumulative factor per datatype
                       __global float			   * aux0_cum,          // array stoing the cumulative factor over all the datatypes
                       __global float              * weight_factor)     // different weight for each datatype
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
        aux0_cum[gid] = aux0_cum[gid] + weight_factor[0]*aux0_factor[gid];
}

#endif // fuse_factors_main

#ifdef EVALUATE_ALPHA
//: function to compute new alpa from the cumulative factors.
__kernel
void evaluate_alpha_main(__constant RenderSceneInfo * linfo,
                         __global uchar16 * trees,
                         __global float* alpha,
                         __global float * aux0_cum,    // array stoing the cumulative factor over all the datatypes
                         __constant uchar * lookup,
                         __local uchar * cumsum,       // for computing data index 
                         __local uchar16 * all_local_tree)
{
    //make sure local_tree points to the right one in shared memory
    uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
    __local uchar16* local_tree    = &all_local_tree[llid];
    __local uchar*   csum          = &cumsum[llid*10];

    //global id should be the same as treeIndex
    unsigned gidX = get_global_id(0);
    unsigned gidY = get_global_id(1);
    unsigned gidZ = get_global_id(2);

    //tree Index is global id
    unsigned treeIndex = calc_blkunisgned(gidX, gidY, gidZ, linfo->dims);
    if (gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z) {
        int MAX_INNER_CELLS, MAX_CELLS;
        get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);
        //1. get current tree information
        (*local_tree)    = trees[treeIndex];
        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < MAX_CELLS; ++i) {
            if ( is_leaf(local_tree, i) ) {
                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);
                //if you've collected a nonzero amount of probs, update it
                int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
                float p = 1/(1+exp(-aux0_cum[currIdx]));
                alpha[currIdx] = -(log(1-p))/ (side_len * linfo->block_len) ;
                 //END LEAF CODE
                ///////////////////////////////////////
            } //end leaf IF
        } //end leaf for
    } //end global id IF
}

#endif


#ifdef HMAP_DENSITY_CELL
typedef struct
{
    __global float * pheight;
    __global float * seg_len;
    __global float * hmean;
    __global float * hvar;
    __constant RenderSceneInfo * linfo;

} AuxArgs;

void step_cell_hmap_density(AuxArgs aux_args, int data_ptr, uchar llid, float d, float tblock)
{
    float t = tblock * aux_args.linfo->block_len;
    float mu = (*aux_args.hmean);
    float var = (*aux_args.hvar);
    float ph = (exp(-0.5*(t - mu)*(t - mu) / var)) / sqrt(2 * M_PI_F* var);
    AtomicAdd((__global float*) (&aux_args.seg_len[data_ptr]), d );
    AtomicAdd((__global float*) (&aux_args.pheight[data_ptr]), ph*d);
}



//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float, __constant RenderSceneInfo*,
    __global int4*, local uchar16*, constant uchar *, local uchar *, float*, AuxArgs, float tnear, float tfar);

__kernel void
compute_hmap_density_main(__constant  RenderSceneInfo    * linfo,
                 __global    int4               * tree_array,       // tree structure for each block
                 __global    float              * aux_array0,        // four aux arrays strung together
                 __global    float              * aux_array1,        // four aux arrays strung together
                 __constant  uchar              * bit_lookup,       // used to get data_index
                 __global    float4             * ray_origins,
                 __global    float4             * ray_directions,
                 __global    float              * nearfarplanes,
                 __global    uint4              * imgdims,          // dimensions of the input image
                 __global    float              * hmean,        // mean image
                 __global    float              * hvar,         // var image
                 __global    float              * output,
                 __local     uchar16            * local_tree,       // cache current tree into local memory
                 __local     uchar              * cumsum)           // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*imgdims).z || j >= (*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;

    //vis for cast_ray, never gets decremented so no cutoff occurs
    float vis = 1.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[j*get_global_size(0) + i];
    float4 ray_d = ray_directions[j*get_global_size(0) + i];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo = linfo;

    aux_args.seg_len = aux_array0;
    aux_args.pheight = aux_array1;
    aux_args.hmean = &(hmean[j*get_global_size(0) + i]);
    aux_args.hvar = &(hvar[j*get_global_size(0) + i]);
    float nearplane = nearfarplanes[0] / linfo->block_len;
    float farplane = nearfarplanes[1] / linfo->block_len;

    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, nearplane, farplane);    //utility info

}
#endif // SEGLEN_HMAP_CELL
#ifdef PRE_HMAP_CELL
typedef struct
{
    __global float* alpha;
    __global float * seg_len;
    __global float * pheight;
    __global float * pre;
    __global float * vis;
    float* vis_inf;
    float* pre_inf;
    __constant RenderSceneInfo * linfo;

} AuxArgs;

void step_cell_pre_hmap(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    ////cell data, i.e., alpha and app model is needed for some passes
    //: pre is updated in normal order.
    //: precurr = pre_previous + pdata_curr* pcurr*vis_curr
    float  alpha = aux_args.alpha[data_ptr];

    float pheight = aux_args.pheight[data_ptr];// / cum_len;
    float seg_len = d*aux_args.linfo->block_len;

    /* Calculate pre and vis infinity */
    float diff_omega = exp(-alpha * seg_len);
    float vis_prob_end = (*aux_args.vis_inf) * diff_omega;

    /* updated pre                      Omega         *   PI  */
    (*aux_args.pre_inf) += ((*aux_args.vis_inf) - vis_prob_end) *  pheight;

    /* updated visibility probability */
    (*aux_args.vis_inf) = vis_prob_end;
    AtomicAdd((__global float*) (&aux_args.seg_len[data_ptr]), d);
    AtomicAdd((__global float*) (&aux_args.pre[data_ptr]), (*aux_args.pre_inf)*d );
    AtomicAdd((__global float*) (&aux_args.vis[data_ptr]), (*aux_args.vis_inf)*d );
}



//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float, __constant RenderSceneInfo*,
    __global int4*, local uchar16*, constant uchar *, local uchar *, float*, AuxArgs, float tnear, float tfar);

__kernel void
pre_hmap_main(__constant  RenderSceneInfo    * linfo,
__global    int4               * tree_array,       // tree structure for each block
__global    float              * alpha_array,      // alpha for each block
__global    float              * aux_array0,        // seg_len
__global    float              * aux_array1,        // pheight_smooth
__global    float              * aux_array2,        // pre
__global    float              * aux_array3,        // vis
__constant  uchar              * bit_lookup,       // used to get data_index
__global    float4             * ray_origins,
__global    float4             * ray_directions,
__global    float              * nearfarplanes,
__global    uint4              * imgdims,          // dimensions of the input image
__global    float              * vis_image,        // visibility image
__global    float              * pre_image,        // preinf image
__global    float              * output,
__local     uchar16            * local_tree,       // cache current tree into local memory
__local     uchar              * cumsum)           // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*imgdims).z || j >= (*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    //float4 inImage = in_image[j*get_global_size(0) + i];
    float vis_inf = vis_image[j*get_global_size(0) + i];
    float pre_inf = pre_image[j*get_global_size(0) + i];

    //vis for cast_ray, never gets decremented so no cutoff occurs
    float vis = 1.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[j*get_global_size(0) + i];
    float4 ray_d = ray_directions[j*get_global_size(0) + i];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo = linfo;
    aux_args.alpha = alpha_array;
    aux_args.seg_len = aux_array0;
    aux_args.pheight = aux_array1;
    aux_args.pre = aux_array2;
    aux_args.vis = aux_array3;
    aux_args.vis_inf = &vis_inf;
    aux_args.pre_inf = &pre_inf;

    float nearplane = nearfarplanes[0] / linfo->block_len;
    float farplane = nearfarplanes[1] / linfo->block_len;

    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, nearplane, farplane);    //utility info

    //store the vis_inf/pre_inf in the image
    vis_image[j*get_global_size(0) + i] = vis_inf;
    pre_image[j*get_global_size(0) + i] = pre_inf;
}

__kernel
void normalize_prehmap_main(__global RenderSceneInfo  * info,
                            __global float              * aux_array0,   // seg_len
                            __global float              * aux_array1,   // pre term
                            __global float              * aux_array2)   // vis term
{
    int gid = get_global_id(0);
    int datasize = info->data_len;
    if (gid < datasize)
    {
        float cum_len = aux_array0[gid];
        if (cum_len > 1e-7)
        {
            aux_array1[gid] = aux_array1[gid] / cum_len;
            aux_array2[gid] = aux_array2[gid] / cum_len;
        }
        else
        {
            aux_array1[gid] = 0.0f;
            aux_array2[gid] = 0.0f;
        }
    }
}

#endif // PREINF

#ifdef POST_HMAP_CELL
typedef struct
{
    __global float* alpha;
    __global float * seg_len;
    __global float * pheight;
    __global float * post;
    float* vis_inf;
    float* post_inf;
    __constant RenderSceneInfo * linfo;

} AuxArgs;


void step_cell_post_hmap(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes
    //: post is updated in reverse order.
    //: postcurr = postprev* \deltavis_curr + pdata_curr* pcurr
    float  alpha = aux_args.alpha[data_ptr];
    float pheight = aux_args.pheight[data_ptr];
    float seg_len = d*aux_args.linfo->block_len;

    /* Calculate pre and vis infinity */
    float diff_omega = exp(-alpha * seg_len);
    AtomicAdd((__global float*) (&aux_args.seg_len[data_ptr]), d);
    AtomicAdd((__global float*) (&aux_args.post[data_ptr]), (*aux_args.post_inf)*d );
      /* updated post = post*(1-P)+ P*PI                     */
    (*aux_args.post_inf) = (*aux_args.post_inf)*diff_omega + (1 - diff_omega)*  pheight;

}
//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float, __constant RenderSceneInfo*,
    __global int4*, local uchar16*, constant uchar *, local uchar *, float*, AuxArgs, float tnear, float tfar);

__kernel void
post_hmap_main(__constant  RenderSceneInfo    * linfo,
__global    int4               * tree_array,       // tree structure for each block
__global    float              * alpha_array,      // alpha for each block
__global    float              * aux_array0,        // seglen
__global    float              * aux_array1,        // heightdensity
__global    float              * aux_array2,        // post
__constant  uchar              * bit_lookup,       // used to get data_index
__global    float4             * ray_origins,
__global    float4             * ray_directions,
__global    float              * nearfarplanes,
__global    uint4              * imgdims,          // dimensions of the input image
__global    float              * vis_image,        // visibility image
__global    float              * post_image,        // preinf image
__global    float              * output,
__local     uchar16            * local_tree,       // cache current tree into local memory
__local     uchar              * cumsum)           // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*imgdims).z || j >= (*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    //float4 inImage = in_image[j*get_global_size(0) + i];
    float vis_inf = vis_image[j*get_global_size(0) + i];
    float post_inf = post_image[j*get_global_size(0) + i];


    //vis for cast_ray, never gets decremented so no cutoff occurs
    float vis = 1.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[j*get_global_size(0) + i];
    float4 ray_d = ray_directions[j*get_global_size(0) + i];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo = linfo;
    aux_args.alpha = alpha_array;
    aux_args.seg_len = aux_array0;
    aux_args.pheight = aux_array1;
    aux_args.post    = aux_array2;

    aux_args.vis_inf = &vis_inf;
    aux_args.post_inf = &post_inf;

    float nearplane = nearfarplanes[0] / linfo->block_len;
    float farplane = nearfarplanes[1] / linfo->block_len;

    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, nearplane, farplane);    //utility info

    //store the vis_inf/pre_inf in the image
    vis_image[j*get_global_size(0) + i] = vis_inf;
    post_image[j*get_global_size(0) + i] = post_inf;
}
#endif // POST_HMAP_CELL

#ifdef COMPUTE_IMAGEDEN
//: fucntion to compute image density for each voxel per image using a MOG3 model
__kernel
void compute_imageden_main(__global RenderSceneInfo  * info,
                           __global MOG_TYPE         * mixture_array,   // mog3 model
                           __global float              * aux_array0,   // seg_len
                           __global float              * aux_array1,   // mean image intensity
                           __global float              * aux_array2)   // image density
{
    int gid = get_global_id(0);
    int datasize = info->data_len;
    if (gid < datasize)
    {
        float cum_len = aux_array0[gid];
        if (cum_len > 1e-7)
        {
            float mean_obs = aux_array1[gid] / cum_len;
            CONVERT_FUNC_FLOAT8(mixture, mixture_array[gid]) / NORM;
            float idensity = gauss_3_mixture_prob_density(mean_obs, (mixture.s0), (mixture.s1), (mixture.s2),
                (mixture.s3), (mixture.s4), (mixture.s5),
                (mixture.s6), (mixture.s7), 1 - (mixture.s2) - (mixture.s5));
            aux_array2[gid] = idensity;
        }
    }
}



#endif // COMPUTE_IMAGEDEN
