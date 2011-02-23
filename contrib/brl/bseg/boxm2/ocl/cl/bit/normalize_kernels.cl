#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER
__kernel void normalize_render_kernel(__global uint * exp_img, 
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
    uchar4 intensity  = as_uchar4(exp_img[imindex]);
    
    //convert uchars to float intensity [0,1] values
    float4 fIntensity = convert_float4(intensity) / 255.0f; 
    fIntensity += (vis*0.5f);   // expected intensity of uniform distribution is 0.5
    
    //convert the intensities back into bytes
    uchar4 post_int = convert_uchar4(fIntensity * 255.0f); 
    exp_img[imindex] = as_uint(post_int);
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
