#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

//#define BLOCK_EPSILON .006125f
//#define TREE_EPSILON  .005f

__kernel
void
ray_trace_bit_scene_opt(__constant  RenderSceneInfo    * linfo,
                        __global    ushort2            * block_ptrs,
                        __global    int4               * tree_array,
                        __global    float              * alpha_array,
                        __global    uchar8             * mixture_array,
                        __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                        __global    uint4              * imgdims,       // dimensions of the image
                        __local     uchar16            * local_tree,
                        __global    float4             * in_image,      // input image and store vis_inf and pre_inf
                        __global    uint               * gl_image, 
                        __constant  uchar              * bit_lookup, 
                        __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                        __local     int                * imIndex,
                        __global    float              * output)    
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
  bool isActive = true;
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    gl_image[imIndex[llid]] = rgbaFloatToInt((float4)(0.0f,0.0f,0.0f,0.0f));
    in_image[imIndex[llid]] = (float4)0.0f;
    isActive = false;
    //return;
  }
  
  if(isActive) {
    //----------------------------------------------------------------------------
    // Calculate ray origin, and direction 
    // (make sure ray direction is never axis aligned)
    //----------------------------------------------------------------------------  
    float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
    float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
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
    float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
    float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;          
    
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------  
    cast_ray( i, j, 
              ray_ox, ray_oy, ray_oz, 
              ray_dx, ray_dy, ray_dz, 

              //scene info                                              //numobs, aux, aux
              linfo, block_ptrs, tree_array, alpha_array, mixture_array, 0, 0, 0, 0, 0, 
             
              //utility info                (factor)
              local_tree, bit_lookup, cumsum, 0,
              
              //RENDER SPECIFIC ARGS
              imIndex,
              
              //io info
              in_image, gl_image, output);
    }
}

