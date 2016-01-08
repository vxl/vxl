#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
#ifdef MOG_TYPE_8
    #define MOG_TYPE uchar8
    #define NORM 255;
#else
    #define MOG_TYPE uchar8
    #define NORM 255;
#endif
#ifdef MOG_TYPE_16
    #define MOG_TYPE ushort8
    #define NORM 65535;
#endif


//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float* vis;
  float* cum_vol;
  float* intensity_norm;
  float* weighted_int;
  float* prob_surface;
  float  volume_scale;
} AuxArgs;

void cast_cone_ray( int i, int j,                                     //pixel information
                    float ray_ox, float ray_oy, float ray_oz,         //ray origin
                    float ray_dx, float ray_dy, float ray_dz,         //ray direction
                    float cone_half_angle,
                    __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                    __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
                    __local     uchar16            * local_tree,      //local tree for traversing
                    __constant  uchar              * bit_lookup,      //0-255 num bits lookup table

                    __constant  float              * centerX,
                    __constant  float              * centerY,
                    __constant  float              * centerZ,

                    __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                    __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread
                                float              * vis,             //passed in as starting visibility
                    AuxArgs aux_args );

__kernel
void
render_expected( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    MOG_TYPE           * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,

                  __constant  float              * centerX,
                  __constant  float              * centerY,
                  __constant  float              * centerZ,

                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                  __local     uchar              * to_visit)      //used as BFS visit list
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;

  //Store image index (may save a register).  Also initialize VIS and expected_int
  int imIndex = j*get_global_size(0)+i;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float cone_half_angle = ray_d.w; ray_d.w = 0.0f;
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float expint  = exp_image[imIndex];
  float vis     = vis_image[imIndex];

  float cum_vol = 0.0f;
  float intensity_norm = 0.0f;;
  float weighted_int = 0.0f;
  float prob_surface = 0.0f;

  //instantiate aux args
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.vis    = &vis;
  aux_args.cum_vol = &cum_vol;
  aux_args.intensity_norm = &intensity_norm;
  aux_args.weighted_int = &weighted_int;
  aux_args.prob_surface = &prob_surface;
  aux_args.volume_scale = linfo->block_len*linfo->block_len*linfo->block_len;

  cast_cone_ray( i, j,
                ray_ox, ray_oy, ray_oz,
                ray_dx, ray_dy, ray_dz, cone_half_angle,
                linfo, tree_array,                                    //scene info
                local_tree, bit_lookup, centerX, centerY, centerZ,
                cumsum, to_visit, &vis, aux_args);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex] =  expint;

  //store visibility at the end of this block
  vis_image[imIndex] = vis;
}

//FUNCTORS used with cone ray
void step_cell_cone(AuxArgs aux_args, int data_ptr, float volume)
{
  volume *= aux_args.volume_scale;

  //grab voxel alpha and intensity
  float alpha = aux_args.alpha[data_ptr];

  //calculate the mean intensity
  uchar8 data = aux_args.mog[data_ptr];
  float w2=0.0f;
  if (data.s2 > 0 && data.s5 > 0)
    w2=(float)(255-data.s2-data.s5);
  float exp_intensity=(float)data.s0 * (float)data.s2 +
                      (float)data.s3 * (float)data.s5 +
                      (float)data.s6 * w2;
  exp_intensity /= (255.0f*255.0f);

  //probability that this voxel is occupied by surface
  float cell_occupancy_prob = (1.0 - exp(-alpha*volume) );
  (*aux_args.prob_surface) += (cell_occupancy_prob * volume);

  //weighted intensity for this voxel
  (*aux_args.weighted_int) += cell_occupancy_prob * volume * exp_intensity;
  (*aux_args.intensity_norm) += cell_occupancy_prob * volume;

  (*aux_args.cum_vol) += volume;
}

//once step cell is performed, compute ball properties
void compute_ball_properties( AuxArgs aux_args )
{
  if( *aux_args.intensity_norm > 1e-10 && *aux_args.cum_vol > 1e-10) {

    //calculate ray/sphere occupancy prob
    float sphere_occ_prob = (*aux_args.prob_surface) / (*aux_args.cum_vol);

    //calc expected int = weighted sum / weighted total volume
    float expected_int = (*aux_args.weighted_int) / (*aux_args.intensity_norm);

    //expected intensity is Visibility * Weighted Intensity * Occupancy
    float ei = (*aux_args.expint);
    float vis = (*aux_args.vis);
    ei += vis * expected_int * sphere_occ_prob;
    (*aux_args.expint) = ei;

    //update visibility after all cells have accounted for
    vis *= (1.0 - sphere_occ_prob);
    (*aux_args.vis) = vis;
  }

  *aux_args.cum_vol = 0.0f;
  *aux_args.intensity_norm = 0.0f;
  *aux_args.weighted_int = 0.0f;
  *aux_args.prob_surface = 0.0f;
}
