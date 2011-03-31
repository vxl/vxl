#ifdef RENDER_HEIGHT_MAP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha; 
  float* expdepth;
  float* expdepthsqr;
  float* probsum;
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float, 
              __constant RenderSceneInfo*, __global int4*, 
              __local uchar16*, __constant uchar *,__local uchar *, 
              float*, AuxArgs); 
__kernel
void
render_height_map( __constant  RenderSceneInfo    * linfo,
                  __global    float               *z,
                  __global    float               *xint,
                  __global    float               *yint,
                  __global    float              * scene_origin,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    float              * height_map,          // input image and store vis_inf and pre_inf
                  __global    float              * height_var_map,      // sum of squares.
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __global    float              * prob_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w) 
    return;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float ray_ox=scene_origin[0]+((float)i+0.5)*(*xint);
  float ray_oy=scene_origin[1]+((float)j+0.5)*(*yint);
  float ray_oz=(*z);
  float ray_dx=0, ray_dy=0, ray_dz=-1;
  
  float4 ray_o =(float4) (ray_ox,ray_oy,ray_oz,1.0);
  float4 ray_d =(float4) (ray_dx,ray_dy,ray_dz,1.0);

  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f);
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0f; ray_d = normalize(ray_d);

  //store float 3's
  ray_ox = ray_o.x;     ray_oy = ray_o.y;     ray_oz = ray_o.z;
  ray_dx = ray_d.x;     ray_dy = ray_d.y;     ray_dz = ray_d.z;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //Store image index (may save a register).  Also initialize VIS and expected_int
  imIndex[llid] = j*get_global_size(0)+i;

  float expdepth   = 0.0f;
  float expdepthsqr= 0.0f;
  float probsum =prob_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args; 
  aux_args.alpha  = alpha_array; 
  aux_args.expdepth = &expdepth;
  aux_args.expdepthsqr = &expdepthsqr;
  aux_args.probsum = &probsum;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);      //utility info
            
  //store the expected intensity 
  height_map[imIndex[llid]] += (* aux_args.expdepth)*linfo->block_len;
  height_var_map[imIndex[llid]] += (* aux_args.expdepthsqr)*linfo->block_len*linfo->block_len;
  prob_image[imIndex[llid]] = (* aux_args.probsum);
  //store visibility at the end of this block
  vis_image[imIndex[llid]]  = vis;
}
#endif