//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef VIS_SEGLEN
//Update step cell functor::seg_len

typedef struct
{
    __global int* seg_len;
    __global int* mean_vis;
    __global float * alpha;
    __constant RenderSceneInfo * linfo;
    float * nearplane;
    float * farplane;
    float * rayvis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel void vis_seg_len_main(__constant  RenderSceneInfo    * linfo,
                               __global    int4               * tree_array,       // tree structure for each block
                               __global    float              * alpha_array,      // alpha for each block
                               __global    int                * aux_array0,       // aux data array (four aux arrays strung together)
                               __global    int                * aux_array2,       // aux data array (four aux arrays strung together)
                               __constant  uchar              * bit_lookup,       // used to get data_index
                               __global    float4             * ray_origins,
                               __global    float4             * ray_directions,
                               __global    float              * nearfarplanes,
                               __global    uint4              * imgdims,          // dimensions of the input image
                               __global    float              * vis_image,         // the input image
                               __global    int                * mask_image,         // the input image
                               __global    float              * output,
                               __local     uchar16            * local_tree,       // cache current tree into local memory
                               __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
                               __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
                               __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
                               __local     uchar              * cumsum    )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //initialize pre-broken ray information (non broken rays will be re initialized)

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;
    //grab input image value (also holds vis)
    float vis1 = vis_image[imIndex];
    float vis = 1.0;
    barrier(CLK_LOCAL_MEM_FENCE);
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;

    if(mask_image[imIndex] == 0 )
        return;

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ imIndex ];
    float4 ray_d = ray_directions[ imIndex ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo    = linfo;
    aux_args.alpha    = alpha_array;
    aux_args.seg_len  = aux_array0;
    aux_args.mean_vis = aux_array2;
    aux_args.rayvis = &vis1;
    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;


    cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    vis_image[imIndex] = vis1;
}
#endif // SEGLEN

#ifdef UPDATE_MAX_VIS_SCORE
// Update each cell using its aux data
//

__kernel void
update_max_vis_score(__global RenderSceneInfo  * info,
                     __global float            * aux_vis_array,
                     __global int              * aux_array0,
                     __global int              * aux_array2,
                     __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {
        //if alpha is less than zero don't update
        float  vis_score    = aux_vis_array[gid];
        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int);
        int vis_int = aux_array2[gid];
        float cell_vis  = convert_float(vis_int);

        float vis = 0.0f;
        if(cum_len > 1e-10f)
            vis = cell_vis/cum_len;

        aux_vis_array[gid] = vis_score > vis ? vis_score:vis;

         aux_array0[gid] = 0;
         aux_array2[gid] = 0;
    }
}

#endif // UPDATE_MAX_VIS_SCORE

#ifdef VIEW_NORMAL_DOT


typedef struct
{
    __global int* expectation_vis_array;
    __global int* expectation_dot_array;
    __global int* seg_len_array;
    __global float * alpha;
    __global float4 * normals;
    float4 ray_dir;
    __constant RenderSceneInfo * linfo;
    float * rayvis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
void step_cell_view_normal_dot(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float vis = (* aux_args.rayvis);

    float alphav = aux_args.alpha[data_ptr];
    float p = 1 - exp(-alphav*d*aux_args.linfo->block_len);
    float4 normal = aux_args.normals[data_ptr];

    float dotp = fabs(normal.x * aux_args.ray_dir.x +
                      normal.y * aux_args.ray_dir.y +
                      normal.z * aux_args.ray_dir.z );

    {
       float factor = exp(-alphav*d*aux_args.linfo->block_len);
        ////SLOW and accurate method
       int seg_int = convert_int_rte(d *  SEGLEN_FACTOR);
       atom_add(&aux_args.seg_len_array[data_ptr], seg_int);
       int vis_int = convert_int_rte(d * vis* SEGLEN_FACTOR);
       atom_add(&aux_args.expectation_vis_array[data_ptr], vis_int);
       int dot_vis = convert_int_rte(d * vis * dotp * SEGLEN_FACTOR);
       atom_add(&aux_args.expectation_dot_array[data_ptr], dot_vis);
       (* aux_args.rayvis) = vis*factor;
    }
}
__kernel void view_normal_dot_main(__constant  RenderSceneInfo    * linfo,
                               __global    int4               * tree_array,       // tree structure for each block
                               __global    float               * alpha_array,      // alpha for each block
                               __global    float4              * normal_array,
                               __global    int                * aux0_array,       // for seg_len
                               __global    int                * aux1_array,       // aux data array (for dot product * seg_len * visibility)
                               __global    int                * aux2_array,       // aux data array (for seg_len * visibility)
                               __constant  uchar              * bit_lookup,       // used to get data_index
                               __global    float4             * ray_origins,
                               __global    float4             * ray_directions,
                               __global    float              * nearfarplanes,
                               __global    uint4              * imgdims,          // dimensions of the input image
                               __global    float              * vis_image,         // the input image
                               __global    int                * mask_image,         // the input image
                               __global    float              * output,
                               __local     uchar16            * local_tree,       // cache current tree into local memory
                               __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
                               __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
                               __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
                               __local     uchar              * cumsum    )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //initialize pre-broken ray information (non broken rays will be re initialized)

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;
    //grab input image value (also holds vis)
    float vis1 = vis_image[imIndex];
    float vis = 1.0;
    barrier(CLK_LOCAL_MEM_FENCE);
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;

    if(mask_image[imIndex] == 0 )
        return;
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ imIndex ];
    float4 ray_d = ray_directions[ imIndex ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo    = linfo;
    aux_args.alpha    = alpha_array;
    aux_args.normals    = normal_array;
    aux_args.seg_len_array          = aux0_array;
    aux_args.expectation_vis_array  = aux1_array;
    aux_args.expectation_dot_array  = aux2_array;
    aux_args.rayvis = &vis1;
    aux_args.ray_dir = (float4)(ray_dx, ray_dy, ray_dz,0.0);
    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

    cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    vis_image[imIndex] = vis1;
}
#endif // SEGLEN

#ifdef UPDATE_AVG_VIEW_NORMAL_DOT
// Update each cell using its aux data
//

__kernel void
update_avg_view_normal_dot(__global RenderSceneInfo  * info,
                           __global int              * aux_array0,
                           __global int              * aux_array1,
                           __global int              * aux_array2,
                           __global float            * expectation_array,
                           __global float            * vis_array,
                           __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {

        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int);
        int vis_int = aux_array1[gid];
        float cell_vis  = convert_float(vis_int);
        int dot_int = aux_array2[gid];
        float cell_dot  = convert_float(dot_int);

        if(cum_len > 1e-10f)
        {
            float vis = cell_vis/cum_len;
            float dot = cell_dot/cum_len;
            float exp_accum = expectation_array[gid];float vis_accum = vis_array[gid];
            if( vis_accum >  1e-10f)
            {
                expectation_array[gid] = dot;

            vis_array[gid] =  vis;
            }
            else if( exp_accum/ vis_accum < dot/vis )
            {
            expectation_array[gid] =  dot;
            //float vis_accum = vis_array[gid];
            vis_array[gid] =  vis;
            }
        }

        aux_array0[gid] = 0;
        aux_array1[gid] = 0;
        aux_array2[gid] = 0;
    }
}

#endif // UPDATE_AVG_VIEW_NORMAL_DOT



#ifdef ACCUMULATE_SURFACE_DENSITY
typedef struct
{
    __global int* expectation_vis_array;
    __global int* expectation_density_array;
    __global int* seg_len_array;
    __global float * alpha;
    __constant RenderSceneInfo * linfo;
    float * rayvis;
    float exp_depth;
    float std_depth;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
void step_cell_surface_density(float depth , AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float vis = (* aux_args.rayvis);
    float ratio=(depth*aux_args.linfo->block_len  - aux_args.exp_depth)/sqrt(aux_args.std_depth);
    float density = exp(-0.5f*ratio*ratio);
    float alphav = aux_args.alpha[data_ptr];

    float factor = exp(-alphav*d*aux_args.linfo->block_len);
    ////SLOW and accurate method
    int seg_int = convert_int_rte(d *  SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len_array[data_ptr], seg_int);
    int vis_int = convert_int_rte(d * vis* SEGLEN_FACTOR);
    atom_add(&aux_args.expectation_vis_array[data_ptr], vis_int);
    int density_vis = convert_int_rte(d * vis * density * SEGLEN_FACTOR);
    atom_add(&aux_args.expectation_density_array[data_ptr], density_vis);
    (* aux_args.rayvis) = vis*factor;
}
__kernel void accumulate_surface_density_main(__constant  RenderSceneInfo    * linfo,
                                              __global    int4               * tree_array,       // tree structure for each block
                                              __global    float              * alpha_array,      // alpha for each block
                                              __global    int                * aux0_array,       // for seg_len
                                              __global    int                * aux1_array,       // aux data array (vis*d)
                                              __global    int                * aux2_array,       // aux data array (density*vis*d)
                                              __constant  uchar              * bit_lookup,       // used to get data_index
                                              __global    float4             * ray_origins,
                                              __global    float4             * ray_directions,
                                              __global    float              * exp_depth_img,
                                              __global    float              * std_depth_img,
                                              __global    float              * nearfarplanes,
                                              __global    uint4              * imgdims,          // dimensions of the input image
                                              __global    float              * vis_image,         // the input image
                                              __global    float              * output,
                                              __local     uchar16            * local_tree,       // cache current tree into local memory
                                              __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
                                              __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
                                              __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
                                              __local     uchar              * cumsum    )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //initialize pre-broken ray information (non broken rays will be re initialized)

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;
    //grab input image value (also holds vis)
    float vis1 = vis_image[imIndex];
    float exp_depth = exp_depth_img[imIndex];
    float std_depth = std_depth_img[imIndex];
    float vis = 1.0;
    barrier(CLK_LOCAL_MEM_FENCE);
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;


    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ imIndex ];
    float4 ray_d = ray_directions[ imIndex ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo    = linfo;
    aux_args.alpha    = alpha_array;

    aux_args.seg_len_array              = aux0_array;
    aux_args.expectation_vis_array      = aux1_array;
    aux_args.expectation_density_array  = aux2_array;
    aux_args.rayvis = &vis1;
    aux_args.exp_depth  = exp_depth;
    aux_args.std_depth  = std_depth;

    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

    cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    vis_image[imIndex] = vis1;
}
#endif // SEGLEN
#ifdef UPDATE_AVG_SURFACE_DENSITY
// Update each cell using its aux data
//

__kernel void
update_avg_surface_density(__global RenderSceneInfo  * info,
                           __global int              * aux_array0,
                           __global int              * aux_array1,
                           __global int              * aux_array2,
                           __global float            * density_array,
                           __global float            * vis_array,
                           __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {

        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int);
        int vis_int = aux_array1[gid];
        float cell_vis  = convert_float(vis_int);
        int density_int = aux_array2[gid];
        float cell_density  = convert_float(density_int);

        if(cum_len > 1e-10f)
        {
            float vis = cell_vis/cum_len;
            float density = cell_density/cum_len;
            float exp_accum = density_array[gid];
            density_array[gid] = exp_accum + density;
            float vis_accum = vis_array[gid];
            vis_array[gid] = vis_accum + vis;
        }

        aux_array0[gid] = 0;
        aux_array1[gid] = 0;
        aux_array2[gid] = 0;
    }
}

#endif // UPDATE_AVG_SURFACE_DENSITY