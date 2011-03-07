#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER
__kernel void normalize_render_kernel(__global float * exp_img, 
                                      __global float* vis_img, 
                                      __global uint4* imgdims)
{
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;
   
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w) 
        return;

    //normalize image with respect to visibility
    float vis   = vis_img[imindex];
    exp_img[imindex] =exp_img[imindex]+ (vis*0.5f);
}
#endif
#ifdef RENDER_GL
__kernel void render_kernel_gl(__constant float *min_i,
                               __constant float *max_i,
                               __constant float *tf,
                               __global float   *vis_img,
                               __global float   *exp_img,                               
                               __global uint    *out_img,
                               __global uint4   *imgdims)
{

    
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i<(*imgdims).x || j<(*imgdims).y|| i>=(*imgdims).z || j>=(*imgdims).w) 
        return;

    
    float intensity  = exp_img[imindex];
    float range=(*max_i-*min_i);
    intensity+=(vis_img[imindex]*((*max_i+*min_i)/2));
    //intensity=clamp(intensity,*mini,*maxi);
    int index=(int)max(floor((intensity-*min_i)/range*255.0f),0.0f);
    out_img[imindex] =rgbaFloatToInt((float4 )tf[index]);//(intensity-*min_i)/range) ;
}
#endif
#ifdef CHANGE
__kernel void normalize_change_kernel(__global uint * exp_img /* background probability density*/ , 
                                      __global uint * prob_exp_img ,
                                      __global float* vis_img, 
                                      __global uint4* imgdims)
{
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);


    int imindex=j*get_global_size(0)+i;
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w) {
        return;
    }
    float vis   = vis_img[imindex];

    uint  eint  = as_uint(exp_img[imindex]);
    uchar echar = convert_uchar(eint);
    float prob_int = convert_float(echar)/255.0f;

    prob_int=1/prob_int -1;
    prob_int+=vis;

    uint  prob_eint  = as_uint(prob_exp_img[imindex]);
    uchar prob_echar = convert_uchar(prob_eint);
    float prob_exp_int = convert_float(prob_echar)/255.0f;
 
    prob_exp_int=1/prob_exp_int -1;
    prob_exp_int+=vis;

    float fgbelief=1.0f/(1.0f+prob_int)-0.5f*min(prob_int,1/prob_int);
    float raybelief=prob_exp_int/(1.0f+prob_exp_int)-0.5*min(prob_exp_int,1/prob_exp_int);
    exp_img[imindex]=rgbaFloatToInt(raybelief*fgbelief);
}
#endif
