#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef CHANGE
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE *  mog;
  float intensity;
  float intensity_exp;
  float* change;
  float* change_exp;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,constant RenderSceneInfo*,
              global int4*,local uchar16*,constant uchar*,local uchar*, float*, AuxArgs);

__kernel
void
change_detection_bit_scene( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    float              * alpha_array,
                            __global    MOG_TYPE           * mixture_array,
                            __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                            __global    float              * in_image,      // input image and store vis_inf and pre_inf
                            __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                            __global    float              * change_image,      // input image and store vis_inf and pre_inf
                            __global    float              * change_exp_image,       //input image
                            __global    uint4              * exp_image_dims,
                            __global    float              * output,
                            __constant  uchar              * bit_lookup,
                            __global    float              * vis_image,
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
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w) {
    return;
  }

  //change, change_exp
  float change          = change_image[imIndex[llid]];
  float change_exp      = change_exp_image[imIndex[llid]];
  float intensity_exp   = exp_image[imIndex[llid]];
  float intensity       = in_image[imIndex[llid]];
  float vis             = vis_image[imIndex[llid]];

  //-----------------------------
  // Do two pass change detection
  //-----------------------------
  //first pass is just one loop
/*
  { 
    //find neighbor pixel
    int currI = i; 
    int currJ = j; 
      
    //calc scene ray
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray(linfo, camera, currI, currJ, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    AuxArgs aux_args;
    aux_args.alpha        = alpha_array;
    aux_args.mog          = mixture_array;
    aux_args.intensity    = intensity;
    aux_args.intensity_exp= intensity_exp;
    aux_args.change       = &change;
    aux_args.change_exp   = &change_exp;

    cast_ray( currI, currJ,
              ray_ox, ray_oy, ray_oz,
              ray_dx, ray_dy, ray_dz,

              //scene info
              linfo, tree_array,

              //utility info
              local_tree, bit_lookup, cumsum, &vis,

              //RENDER SPECIFIC ARGS
              aux_args);
              
    //expected image gets rendered
    change_image[imIndex[llid]]     = change;  //expected_int;
    change_exp_image[imIndex[llid]] = change_exp; //expected_int;
    vis_image[imIndex[llid]]        = vis;
  }
  
*/
  //Second pass, do 3 loops on those who's change thresholds are hight enough
  float prob_change = 1.0f / (1.0f + change+vis); 
  //if( prob_change > 0.1f ) 
  {
    //change max and change exp max
    float change_sum = 0.0f; 
    float change_exp_sum = 0.0f; 
    float vis_out = 0.0f; 

    //cast a ray for each pixel in a 3x3 neighborhood
    for(int oi=-0; oi<=0; ++oi) 
    {
      for(int oj=-0; oj<=0; ++oj) 
      {
        //find neighbor pixel
        int currI = i + oi; 
        int currJ = j + oj; 
        if (currI>=(*exp_image_dims).z || currJ>=(*exp_image_dims).w) 
          continue;
          
        //calc scene ray
        float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
        calc_scene_ray(linfo, camera, currI, currJ, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
      
        change          = change_image[imIndex[llid]];
        change_exp      = change_exp_image[imIndex[llid]];
        intensity_exp   = exp_image[imIndex[llid]];
        intensity       = in_image[imIndex[llid]];
        vis             = vis_image[imIndex[llid]];
        
        AuxArgs aux_args;
        aux_args.alpha        = alpha_array;
        aux_args.mog          = mixture_array;
        aux_args.intensity    = intensity;
        aux_args.intensity_exp= intensity_exp;
        aux_args.change       = &change;
        aux_args.change_exp   = &change_exp;

        cast_ray( currI, currJ,
                  ray_ox, ray_oy, ray_oz,
                  ray_dx, ray_dy, ray_dz,

                  //scene info
                  linfo, tree_array,

                  //utility info
                  local_tree, bit_lookup, cumsum, &vis,

                  //RENDER SPECIFIC ARGS
                  aux_args);
                  
         //change_sum += change; 
         //change_exp_sum += change_exp; 
         if( oi==0 && oj==0 ) vis_out = vis; 
         change_sum = max(change, change_sum); 
         change_exp_sum = max(change_exp, change_exp_sum); 
      }       
    }
    //expected image gets rendered
    change_image[imIndex[llid]]     = change_sum;  //expected_int;
    change_exp_image[imIndex[llid]] = change_exp_sum; //expected_int;
    vis_image[imIndex[llid]]        = vis_out;
  }

/*
  AuxArgs aux_args;
  aux_args.alpha        = alpha_array;
  aux_args.mog          = mixture_array;
  aux_args.intensity    = intensity;
  aux_args.intensity_exp= intensity_exp;
  aux_args.change       = &change;
  aux_args.change_exp   = &change_exp;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,

            //scene info
            linfo, tree_array,

            //utility info
            local_tree, bit_lookup, cumsum, &vis,

            //RENDER SPECIFIC ARGS
            aux_args);

  //expected image gets rendered
  change_image[imIndex[llid]]     =  change; //expected_int;
  change_exp_image[imIndex[llid]] =  change_exp; //expected_int;
  vis_image[imIndex[llid]]        = vis;
*/
}

#endif
