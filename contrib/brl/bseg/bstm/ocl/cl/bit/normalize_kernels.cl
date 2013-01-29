#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef NORMALIZE_RENDER_GL
__kernel void normalize_render_kernel_gl(__global uint * exp_img,
                                         __global float* vis_img,
                                         __global uint4* imgdims)
{
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i<(*imgdims).x || j<(*imgdims).y|| i>=(*imgdims).z || j>=(*imgdims).w)
        return;

    float intensity  = as_float(exp_img[imindex]);
    float vis = vis_img[imindex];
    intensity += vis*.5f;
    exp_img[imindex] =(rgbaFloatToInt((float4) intensity));//(intensity-*min_i)/range) ;
}

__kernel void normalize_render_kernel_rgb_gl( __global float4* exp_img,
                                              __global float* vis_img,
                                              __global uint4* imgdims,
                                              __global uint*  gl_im)
{    
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i<(*imgdims).x || j<(*imgdims).y|| i>=(*imgdims).z || j>=(*imgdims).w)
        return;

    float vis = vis_img[imindex];
    float4 intensity = exp_img[imindex] + (vis*0.5f);
    intensity.w = 1.0f;

    gl_im[imindex]   = rgbaFloatToInt( (float4) intensity );
}
#endif



//Old deprecated code.
#if 0
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
    exp_img[imindex] = exp_img[imindex] + (vis*0.5f);
}

__kernel void normalize_render_rgb_kernel(__global float4* exp_img,
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
    exp_img[imindex] = exp_img[imindex]+ (vis*0.5f);
}

#endif
