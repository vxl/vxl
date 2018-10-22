//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef SEGLENNOBS
typedef struct
{
  __global int* seg_len;
  __global uint* nobs;

  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);
__kernel
void
seg_len_nobs_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    int                * aux_array0,       // aux data array (four aux arrays strung together)
             __global    uint               * nobs,             // data array to keep track of the number of observations per cell
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * in_image,         // the input image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
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
  int imIndex = j*get_global_size(0) + i;

  //grab input image value (also holds vis)
  float obs = in_image[imIndex];
  float vis = 1.0f;  //no visibility in this pass
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y || obs < 0.0f )
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
  aux_args.seg_len  = aux_array0;
  aux_args.nobs = nobs;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info


}
#endif // SEGLENNOBS


#ifdef EXPSUM

typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  //__global int* mean_obs;
           float* vis_inf;
           float* exp_denom;
           float  obs;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
exp_sum_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,         // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,     // mixture for each block
             __global    int                * aux_array0,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,        // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,           // dimensions of the input image
             __global    float              * in_image,         // the input image
             __global    float              * vis_image,        // visibility image
             __global    float              * exp_denom_img,    // the denominator of expectation image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
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


  float obs = in_image[j*get_global_size(0) + i];
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;


  float vis_inf = vis_image[j*get_global_size(0) + i];
  float exp_denom = exp_denom_img[j*get_global_size(0) + i];

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
  aux_args.mog       = mixture_array;
  aux_args.seg_len   = aux_array0;
  //aux_args.mean_obs  = aux_array1;
  aux_args.vis_inf   = &vis_inf;
  aux_args.exp_denom = &exp_denom;
  aux_args.obs = obs;


  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info

  vis_image[j*get_global_size(0)+i] = vis_inf;
  exp_denom_img[j*get_global_size(0)+i] = exp_denom;
}
#endif // EXPSUM

#ifdef REINIT_VIS
__kernel
void
reinit_vis_image (  __global    uint4               * imgdims,          // dimensions of the input image
                    __global    float               * in_image,         // the input image
                    __global    float               * vis_image,
                    __global    float               * exp_denom_img,
                    __global    float               * pi_sum_img,
                    __global    float4              * app_density )
{
  // linear global id of the normalization image
  int i=get_global_id(0);
  int j=get_global_id(1);

  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  float vis = vis_image[j*get_global_size(0) + i];
  exp_denom_img[j*get_global_size(0)+i] += vis * gauss_prob_density(in_image[j*get_global_size(0) + i] , app_density[0].y,app_density[0].z);
  pi_sum_img[j*get_global_size(0)+i] = vis * gauss_prob_density(in_image[j*get_global_size(0) + i] , app_density[0].y,app_density[0].z);
  vis_image[j*get_global_size(0) + i] = 1.0f; // initial vis = 1.0f
}
#endif // REINIT_VIS



#ifdef EXPECTATION

typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global float* all_seglen;
  __global float* all_preexp;
  __global int* all_exp;
  __global uchar* all_obs;
  __global uint* currIdx;
  __global uint* nobs;
           float* vis_inf;
           float* pre_exp_num;
           float  exp_denom;
           float  pi_inf;
           float  obs;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
expectation_main(__constant  RenderSceneInfo * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    int                * aux_array0,       // seglen
             __global    float              * aux_array2,       // all pre_exp
             __global    float              * aux_array3,       // all_seglen
             __global    uchar              * aux_array4,       // all_obs
             __global    int                * aux_array5,       // all_exp
             __global    uint               * currIdx,          // indices indicating where to begin writing obs/exp for each cell
             __global    uint               * nobs,             // number of obs per cell
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * in_image,         // the input image
             __global    float              * vis_image,        // visibility image
             __global    float              * exp_denom_img,    // exp sum image
             __global    float              * pre_exp_num_image,// pre exp image
             __global    float              * pi_inf_image,     // pi inf image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
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


  float obs = in_image[j*get_global_size(0) + i];
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  float vis_inf = vis_image[j*get_global_size(0) + i];
  float pre_exp_num = pre_exp_num_image[j*get_global_size(0) + i];

  if (vis_inf < 0.0)
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
  aux_args.linfo    = linfo;
  aux_args.alpha    = alpha_array;
  aux_args.mog      = mixture_array;
  aux_args.seg_len  = aux_array0;
  aux_args.all_preexp   = aux_array2;
  aux_args.all_seglen  = aux_array3;
  aux_args.all_obs  = aux_array4;
  aux_args.all_exp  = aux_array5;
  aux_args.currIdx = currIdx;
  aux_args.nobs = nobs;
  aux_args.vis_inf  = &vis_inf;
  aux_args.pre_exp_num = &pre_exp_num;
  aux_args.pi_inf = pi_inf_image[j*get_global_size(0) + i];
  aux_args.exp_denom = exp_denom_img[j*get_global_size(0) + i];
  aux_args.obs = obs;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info

  vis_image[j*get_global_size(0)+i] = vis_inf;
  pre_exp_num_image[j*get_global_size(0)+i] = pre_exp_num;
}
#endif // EXPECTATION


#ifdef CONVERT_AUX
__kernel void
convert_aux_int_to_float(__constant  RenderSceneInfo    * linfo,
                         __global float* aux_array0,
                         __global float* aux_array2,
                         __global float* aux_array3)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len;
  if (gid<datasize)
  {
    int obs0= as_int(aux_array0[gid]);
    int obs2= as_int(aux_array2[gid]);
    int obs3= as_int(aux_array3[gid]);

    aux_array0[gid] =  (convert_float(obs0) / SEGLEN_FACTOR) * linfo->block_len;
    aux_array2[gid] =  (convert_float(obs2) / SEGLEN_FACTOR) * linfo->block_len;
    aux_array3[gid] =  (convert_float(obs3) / SEGLEN_FACTOR) * linfo->block_len;
   }
}
#endif //CONVERT_AUX

#ifdef CONVERT_EXP
__kernel void
convert_exp_to_float(__global int  * datasize,
                     __global float* all_exp )
{
  int gid=get_global_id(0);
  if (gid< (*datasize) )
  {
    int int_exp = as_int(all_exp[gid]);
    if(int_exp  >= 0)
        all_exp[gid] =  convert_float( int_exp ) / SEGLEN_FACTOR;
    else
        all_exp[gid] = -1;
  }
}
#endif //CONVERT_EXP

#ifdef COMPUTE_ALPHA
__kernel
void batch_update_alpha (   __global RenderSceneInfo  * info,
                            __global float * expectations,
                            __global float * seglens,
                            __global int * num_imgs,
                            __global int * datasize,
                            __global float * weighted_pre_exp_sum,
                            __global float * pre_exp_sum,
                            __global float * exp_sum,
                            __global float * seglen_sum,
                            __global float * alpha,
                            __global uint * sampleIndex,
                            __global uint * totalNumSamples
                           )
{
    unsigned gid = get_global_id(0);
    if (gid<(*datasize))
    {

        float  cell_min = info->block_len/(float)(1<<info->root_level);
        float  alphamin = -log(1.0f-0.0001f)/cell_min;

        //calculate the number of samples for this cell
        uint start = sampleIndex[gid];
        uint end   = (gid==(*datasize)-1) ? (*totalNumSamples) : sampleIndex[gid+1];
        int numSamples = (int) (end-start);
        if(numSamples <= 0)
        {
          alpha[gid] = alphamin;
          return;
        }


        constexpr short max_iterations = 500;
        const float TOL = 1e-2f;
        short i = 0;

        float curr_alpha =  alpha[gid];
        if( seglen_sum[gid] > 0 && pre_exp_sum[gid] > 0)
            curr_alpha = (numSamples / seglen_sum[gid]) * ( log( pre_exp_sum[gid] + exp_sum[gid]) - log( pre_exp_sum[gid]) );


        if(curr_alpha < alphamin) {
            alpha[gid] = alphamin;
            return;
        }

        float grad,grad2;
        for(; i < max_iterations;i++)
        {
            grad = -weighted_pre_exp_sum[gid]; grad2 = 0;
            for(unsigned s = 0; s < numSamples; s++) {
                float seglen = seglens[start+s];
                float exponent = exp(curr_alpha*seglen);

                float inc = seglen * expectations[start+s]  / ( exponent-1 );
                if( isfinite(inc) && !isnan(inc))
                    grad +=  inc;

                float inc2 = (-seglen*seglen) * expectations[start+s] / (exponent - 2 +   exp(-curr_alpha*seglen) );
                if( isfinite(inc2) && !isnan(inc2) )
                    grad2 +=  inc2;

            }

            if(!isnan(grad / grad2) && isfinite(grad / grad2) )
            {
                if(fabs(grad / grad2) < TOL && grad2 < 0) //local maxima
                    break;

                if(grad2 < 0)
                    curr_alpha -= grad / grad2;
                else
                    curr_alpha += grad / grad2; //grad ascent
            }
            else
                return;
        }
        alpha[gid] = curr_alpha;
        weighted_pre_exp_sum[gid] = grad;

    }

}
#endif // COMPUTE_ALPHA


#define MAX_SAMPLES  1000

#ifdef COMPUTE_MOG_CPU
__kernel
void batch_update_mog          (__global RenderSceneInfo  * info,
                                __global uchar * aux0,
                                __global float * aux2,
                                __constant int * num_imgs,
                                __constant int * datasize,
                                __global MOG_TYPE * mog,
                                __global ushort4* num_iter,
                                __global uint * sampleIndex,
                                __global uint * totalNumSamples
                               )
{
    unsigned gid = get_global_id(0);
    if (gid<(*datasize))
    {

        //calculate the number of samples for this cell
        uint start = sampleIndex[gid];
        uint end   = (gid==(*datasize)-1) ? (*totalNumSamples) : sampleIndex[gid+1];
        int numSamples = (int) (end-start);
        if(numSamples <= 0)
        {
          mog[gid] = (MOG_TYPE)(0);
          return;
        }

        if(numSamples > MAX_SAMPLES)
            numSamples = MAX_SAMPLES;

        //create local arrays
        uchar intensities[MAX_SAMPLES] = {0};
        float expectations[MAX_SAMPLES] = {0};

        //copy from global mem.
        for(unsigned s = 0; s < numSamples; s++) {
            intensities[s] = aux0[start+s];
        }

        float weights_normalizer = 0.0f;
        for(unsigned s = 0; s < numSamples; s++) {
            expectations[s] = aux2[start+s];
            weights_normalizer += expectations[s];
        }

        //update mog
        float min_sigma = .01;
        float8 new_mog = weighted_mog3_em( intensities, expectations, numSamples, min_sigma, weights_normalizer, &(num_iter[gid]));
        new_mog *= (float) NORM;
        CONVERT_FUNC_SAT_RTE(mog[gid], new_mog);

        num_iter[gid].y = numSamples;
    }

}
#endif // COMPUTE_MOG_CPU


//unused old code.
#if 0
#ifdef COMPUTE_MOG_EXPERIMENTAL
__kernel
void batch_update_mog          (__global RenderSceneInfo  * info,
                                __global uchar16 * aux0, //observations
                                __global float16 * aux2, //expectations
                                __constant int * num_imgs,
                                __constant int * datasize,
                                __global MOG_TYPE * mog,
                                __global ushort4* num_iter,
                                __local uchar16 * intensities,
                                __local float16 * expectations,
                                __local uint * counter
                               )
{

    unsigned gid = get_group_id(0);
    short llid = get_local_id(0) + get_local_size(0)*get_local_id(1);
    if (gid<(*datasize))
    {
        if(llid == 0)
            (*counter) = 0;
        barrier(CLK_LOCAL_MEM_FENCE);

        //copy observations and expectations to local mem
        if (llid < (*num_imgs) )
        {
            __global float * curr_exp = &(aux2[(*datasize)*llid+gid]);
            __global uchar * curr_int = &(aux0[(*datasize)*llid+gid]);

            for (short s = 0; s < 16 && curr_exp[s] >= 0; s++)
            {
                uint my_counter = atomic_inc(counter);
                //intensities[50] = curr_int[s];
                expectations[my_counter] = curr_exp[s];

            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);

      /*
        int no_samples = counter[0];
        no_samples = min(no_samples, MAX_SAMPLES);

        //update mog
        CONVERT_FUNC_FLOAT8(mixture,mog[gid])/NORM;

        float min_sigma = .025;
        unsigned short iter;
        float8 new_mog = weighted_mog3_em( intensities, expectations, no_samples, min_sigma, &mixture, &(num_iter[gid]));
        //float8 new_mog = weighted_gaussian( intensities, expectations, counter, min_sigma);
        new_mog *= (float) NORM;
        CONVERT_FUNC_SAT_RTE(mog[gid], new_mog);
         * */
        num_iter[gid].y = counter;

    }

}
#endif // COMPUTE_MOG



#ifdef COMPUTE_MOG_EXP
__kernel
void batch_update_mog          (__global RenderSceneInfo  * info,
                                __global uchar16 * aux0, //observations
                                __global float16 * aux2, //expectations
                                __constant int * num_imgs,
                                __constant int * datasize,
                                __global MOG_TYPE * mog,
                                __global ushort4* num_iter,
                                __local uchar16 * intensities,
                                __local float16 * expectations,
                                __local float * mode1_probs,
                                __local float * mode2_probs,
                                __local float * mode3_probs,
                                __local uint * counter
                               )
{

    unsigned gid = get_group_id(0);
    short llid = get_local_id(0) + get_local_size(0)*get_local_id(1);
    if (gid<(*datasize))
    {
        //copy observations and expectations to local mem
        if (llid < (*num_imgs) )
        {
            intensities[llid]    = aux0[(*datasize)*llid+gid];
            expectations[llid]   = aux2[(*datasize)*llid+gid];
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        //leader thread sets alpha
        if (llid == 0) {
            uint counter = 0;

            __local float * exp_all = expectations;
            __local uchar * int_all = intensities;

            for (short k = 0; k<(*num_imgs); k++) {
                __local float * curr_exp = &(expectations[llid]);
                __local uchar * curr_int = &(intensities[llid]);

                for (short s = 0; s < 16 && curr_exp[s] >= 0; s++)
                {
                    exp_all[counter] = curr_exp[s];
                    int_all[counter] = curr_int[s];
                    counter++;
                }
            }

            counter = min(counter, MAX_SAMPLES);

            //update mog
            CONVERT_FUNC_FLOAT8(mixture,mog[gid])/NORM;

            float min_sigma = .025;
            unsigned short iter;
            float8 new_mog = weighted_mog3_em( int_all, exp_all, counter, min_sigma,
                                                &mixture, &(num_iter[gid]), mode1_probs, mode2_probs, mode3_probs);
            //float8 new_mog = weighted_gaussian( intensities, expectations, counter, min_sigma);
            new_mog *= (float) NORM;
            CONVERT_FUNC_SAT_RTE(mog[gid], new_mog);

            num_iter[gid].y = counter;

        }
    }

}
#endif // COMPUTE_MOG

#endif

