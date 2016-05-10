    //Render height map kernel, step cell fucntor is in "expected_functor"
//choose the correct MOG type/size
#ifdef RENDER_HEIGHT_MAP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE* mog;
  float* expdepth;
  float* expdepthsqr;
  float* probsum;
  float* expint;
  float* vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs, float tnear, float tfar);
__kernel
void
render_height_map(__constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    MOG_TYPE           * mixture_array,
                  __global    float              * exp_image,
                  __global    float              * height_map,    // input image and store vis_inf and pre_inf
                  __global    float              * height_var_map,// sum of squares.
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __global    float              * prob_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                  __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0) + i;
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[imIndex[llid]];
  float4 ray_d = ray_directions[imIndex[llid]];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  float expdepth   = 0.0f;
  float expdepthsqr= 0.0f;
  float expint  = exp_image[imIndex[llid]];
  float probsum =prob_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  float vis1 = vis;
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog = mixture_array;
  aux_args.expdepth = &expdepth;
  aux_args.expdepthsqr = &expdepthsqr;
  aux_args.probsum = &probsum;
  aux_args.expint = &expint;
  aux_args.vis = &vis1;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args, 0, MAXFLOAT);      //utility info

  //store the expected intensity
  height_map[imIndex[llid]] =height_map[imIndex[llid]]+ ((* aux_args.expdepth)*linfo->block_len);
  height_var_map[imIndex[llid]] =height_var_map[imIndex[llid]]+ ((* aux_args.expdepthsqr)*linfo->block_len*linfo->block_len);
  prob_image[imIndex[llid]] = (* aux_args.probsum);
  //store visibility at the end of this block
  vis_image[imIndex[llid]]  = vis;
  exp_image[imIndex[llid]]  = expint;
}
#endif // RENDER_HEIGHT_MAP

//Render height map kernel, step cell fucntor is in "expected_functor"
//choose the correct MOG type/size
#ifdef INGEST_HEIGHT_MAP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float * alpha;
//  float  outimg;
  float resolution ;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void ingest_height_map(__constant  RenderSceneInfo    * linfo,
                       __global    uint4              * image_dims,
                       __global    float4             * ray_origin_buff,
                       __global    float              * outimg_buff,
                       __global    int4               * tree_array,
                       __global    float              * alpha_array,
                       __constant  uchar              * bit_lookup,
                       __local     uchar16            * local_tree,
                       __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                       __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*image_dims).z || j>=(*image_dims).w)
    return;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origin_buff[ imIndex[llid] ];

  float4 ray_d = (float4)( 0.001,  0.001, -1.0, 1.0);

  float ray_ox = 0.0f;float ray_oy = 0.0f;float ray_oz = 0.0f;
  float ray_dx = 0.0f;float ray_dy = 0.0f;float ray_dz = 0.0f;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d,
                             &ray_ox, &ray_oy, &ray_oz,
                             &ray_dx, &ray_dy, &ray_dz);

  ////----------------------------------------------------------------------------
  //// we know i,j map to a point on the image, have calculated ray
  //// BEGIN RAY TRACE
  ////----------------------------------------------------------------------------

  AuxArgs aux_args;
  float out1 =0.0f;
  aux_args.alpha  = alpha_array;
//  aux_args.outimg = out1;
  float vis =1.0;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

}
#endif // INGEST_HEIGHT_MAP
#ifdef COMPUTE_PROB_HEIGHT_MAP_BP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
    __global float* alpha;
    float* probsum;
    float hmap;
    float var;
    float* vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float,
    __constant RenderSceneInfo*, __global int4*,
    __local uchar16*, __constant uchar *, __local uchar *,
    float*, AuxArgs, float tnear, float tfar);
__kernel void
computer_prob_height_map_bp(__constant  RenderSceneInfo    * linfo,
                            __global    float              * z,
                            __global    float              * xint,
                            __global    float              * yint,
                            __global    float              * scene_origin,
                            __global    int4               * tree_array,
                            __global    float              * alpha_array,
                            __global    float              * height_map,    // input image and store vis_inf and pre_inf
                            __global    float              * height_var_map,// sum of squares.
                            __global    uint4              * exp_image_dims,
                            __global    float              * output,
                            __constant  uchar              * bit_lookup,
                            __global    float              * vis_image,
                            __global    float              * prob_norm_image,
                            __local     uchar16            * local_tree,
                            __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                            __local     int                * imIndex)
{
    //----------------------------------------------------------------------------
    //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*exp_image_dims).z || j >= (*exp_image_dims).w)
        return;

    //----------------------------------------------------------------------------
    // Calculate ray origin, and direction
    // (make sure ray direction is never axis aligned)
    //----------------------------------------------------------------------------
    float ray_ox = scene_origin[0] + ((float)i + 0.5f)*(*xint);
    float ray_oy = scene_origin[1] + ((float)j + 0.5f)*(*yint);
    float ray_oz = (*z);
    float ray_dx = 0, ray_dy = 0, ray_dz = -1;

    float4 ray_o = (float4)(ray_ox, ray_oy, ray_oz, 1.0);
    float4 ray_d = (float4)(ray_dx, ray_dy, ray_dz, 1.0);

    ray_o = ray_o - linfo->origin;  ray_o.w = 1.0f; //translate ray o to zero out scene origin
    ray_o = ray_o / linfo->block_len; ray_o.w = 1.0f;

    //thresh ray direction components - too small a treshhold causes axis aligned
    //viewpoints to hang in infinite loop (block loop)
    float thresh = exp2(-14.0f);
    if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
    if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
    if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
    ray_d.w = 0.0f; ray_d = normalize(ray_d);

    //store float 3's
    ray_ox = ray_o.x;     ray_oy = ray_o.y;     ray_oz = ray_o.z;
    ray_dx = ray_d.x;     ray_dy = ray_d.y;     ray_dz = ray_d.z;

    imIndex[llid] = j*get_global_size(0) + i;


    float hmap = height_map[imIndex[llid]];
    float var = height_var_map[imIndex[llid]];
    float probsum = prob_norm_image[imIndex[llid]];
    float vis = vis_image[imIndex[llid]];
    AuxArgs aux_args;
    aux_args.alpha = alpha_array;
    aux_args.hmap = hmap;
    aux_args.var = var;
    aux_args.probsum = &probsum;
    aux_args.vis = &vis;
    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                    //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, 0, MAXFLOAT);      //utility info
    prob_norm_image[imIndex[llid]] = (*aux_args.probsum);
    //store visibility at the end of this block
    vis_image[imIndex[llid]] = (*aux_args.vis);

}
#endif // COMPUTE_PROB_HEIGHT_MAP_BP
#ifdef INGEST_HEIGHT_MAP_BP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
    __global float* alpha;
    __global float* aux0;
    __global float* aux3;
    float* probsum;
    float probnormsum;
    int i;
    int j;
    int ni;
    int cl_ni;
    int nj;
    __global float *hmapimg;
    __global float *hmapvarimg;
    int numsamples;
    __global int * samples;
    float* vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int, int, float, float, float, float, float, float,
            __constant RenderSceneInfo*, __global int4*,
            __local uchar16*, __constant uchar *, __local uchar *,
            float*, AuxArgs, float tnear, float tfar);
__kernel void
ingest_height_map_bp(__constant  RenderSceneInfo    * linfo,
                     __global    float              * z,
                     __global    float              * xint,
                     __global    float              * yint,
                     __global    float              * scene_origin,
                     __global    int4               * tree_array,
                     __global    float              * alpha_array,
                     __global    float              * aux0_array,
                     __global    float              * aux3_array,
                     __global    float              * height_map,    // input image and store vis_inf and pre_inf
                     __global    float              * height_var_map,// sum of squares.
                     __global    uint4              * exp_image_dims,
                     __global    int                * numsamples,
                     __global    int                * rsamples,
                     __global    float              * output,
                     __constant  uchar              * bit_lookup,
                     __global    float              * vis_image,
                     __global    float              * prob_image,
                     __global    float              * prob_norm_image,
                     __local     uchar16            * local_tree,
                     __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                     __local     int                * imIndex)
{
    //----------------------------------------------------------------------------
    //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    int i = 0, j = 0;
    i = get_global_id(0);
    j = get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i >= (*exp_image_dims).z || j >= (*exp_image_dims).w)
        return;

    //----------------------------------------------------------------------------
    // Calculate ray origin, and direction
    // (make sure ray direction is never axis aligned)
    //----------------------------------------------------------------------------
    float ray_ox = scene_origin[0] + ((float)i + 0.5f)*(*xint);
    float ray_oy = scene_origin[1] + ((float)j + 0.5f)*(*yint);
    float ray_oz = (*z);
    float ray_dx = 0, ray_dy = 0, ray_dz = -1;

    float4 ray_o = (float4)(ray_ox, ray_oy, ray_oz, 1.0);
    float4 ray_d = (float4)(ray_dx, ray_dy, ray_dz, 1.0);

    ray_o = ray_o - linfo->origin;  ray_o.w = 1.0f; //translate ray o to zero out scene origin
    ray_o = ray_o / linfo->block_len; ray_o.w = 1.0f;

    //thresh ray direction components - too small a treshhold causes axis aligned
    //viewpoints to hang in infinite loop (block loop)
    float thresh = exp2(-14.0f);
    if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
    if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
    if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
    ray_d.w = 0.0f; ray_d = normalize(ray_d);

    //store float 3's
    ray_ox = ray_o.x;     ray_oy = ray_o.y;     ray_oz = ray_o.z;
    ray_dx = ray_d.x;     ray_dy = ray_d.y;     ray_dz = ray_d.z;

    imIndex[llid] = j*get_global_size(0) + i;
    float hmap = height_map[imIndex[llid]];
    float var = height_var_map[imIndex[llid]];
    float probsum = prob_image[imIndex[llid]];
    float probnormsum = prob_norm_image[imIndex[llid]];
    float vis = vis_image[imIndex[llid]];
    AuxArgs aux_args;
    aux_args.alpha = alpha_array;
    aux_args.aux0 = aux0_array;
    aux_args.aux3 = aux3_array;
    aux_args.i = i;
    aux_args.j = j;
    aux_args.cl_ni = get_global_size(0);
    aux_args.ni = (*exp_image_dims).z;
    aux_args.nj = (*exp_image_dims).w;
    aux_args.hmapimg = height_map;
    aux_args.hmapvarimg = height_var_map;
    aux_args.numsamples = *numsamples;
    aux_args.samples = rsamples;
    aux_args.probsum = &probsum;
    aux_args.probnormsum = probnormsum;
    aux_args.vis = &vis;
    cast_ray(i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                    //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args, 0, MAXFLOAT);
    prob_image[imIndex[llid]] = (*aux_args.probsum);
    //store visibility at the end of this block
    vis_image[imIndex[llid]] = (*aux_args.vis);
}
#endif // INGEST_HEIGHT_MAP_BP

#ifdef INGEST_HEIGHT_MAP_BP_ALPHA
__kernel void update_hmap_bp(__constant  RenderSceneInfo    * linfo,
                             __global    int4               * tree_array,
                             __global    float              * alpha_array,
                             __global    float              * aux0_array,
                             __global    float              * aux3_array)
{
    int gid = get_global_id(0);
    int datasize = linfo->data_len;
    if (gid<datasize)
    {
        float cumlen = aux0_array[gid];
        if (cumlen > 1e-20f)
        {
            float beta = aux3_array[gid] / cumlen;
            beta = clamp(beta, 0.001f, 100.0f);
            float multiplier = pow(beta, 1.0f);
            alpha_array[gid] = alpha_array[gid] * multiplier;
        }
    }
}
#endif
#ifdef INGEST_BUCKEYE_DEM
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float * belief;
  __global float * uncertainty;
  float  first_depth;
  float  last_depth;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs, float tnear, float tfar);
__kernel
void ingest_buckeye_dem(__constant  RenderSceneInfo    * linfo,
                       __global    uint4              * image_dims,
                       __global    float4             * ray_origin_buff,
                       __global    float              * a1_buff,
                       __global    float              * a2_buff,
                       __global    int4               * tree_array,
                       __global    float              * aux0_array,
                       __global    float              * aux1_array,
                       __constant  uchar              * bit_lookup,
                       __local     uchar16            * local_tree,
                       __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                       __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*image_dims).z || j>=(*image_dims).w)
    return;

  float first_depth = a1_buff[imIndex[llid]];
  float last_depth = a2_buff[imIndex[llid]];

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origin_buff[ imIndex[llid] ];

  float4 ray_d = (float4)( 0.001,  0.001, -1.0, 1.0);

  float ray_ox = 0.0f;float ray_oy = 0.0f;float ray_oz = 0.0f;
  float ray_dx = 0.0f;float ray_dy = 0.0f;float ray_dz = 0.0f;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d,
                             &ray_ox, &ray_oy, &ray_oz,
                             &ray_dx, &ray_dy, &ray_dz);

  ////----------------------------------------------------------------------------
  //// we know i,j map to a point on the image, have calculated ray
  //// BEGIN RAY TRACE
  ////----------------------------------------------------------------------------

  AuxArgs aux_args;

  aux_args.belief  = aux0_array;
  aux_args.uncertainty = aux1_array;
  // This happens sometimes, not sure why.
  if (first_depth > last_depth) {
    aux_args.first_depth = last_depth;
    aux_args.last_depth = first_depth;
  }
  else {
    aux_args.first_depth = first_depth;
    aux_args.last_depth = last_depth;
  }
  float vis = 1.0;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

}
#endif // INGEST_BUCKEYE_DEM
