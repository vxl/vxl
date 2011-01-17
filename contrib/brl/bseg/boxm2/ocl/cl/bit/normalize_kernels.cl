#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER
__kernel void normalize_render_kernel(__global uint * exp_img , __global float* vis_img, __global    uint4    * imgdims)
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
    float expected_int = convert_float(echar)/255.0f;

    expected_int+=(vis*0.5); // expected intensity of uniform distribution is 0.5

    exp_img[imindex]=rgbaFloatToInt(expected_int);
}
#endif

#ifdef CHANGE
__kernel void normalize_change_kernel(__global uint * exp_img , __global float* vis_img, __global    uint4    * imgdims)
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
    float expected_int = convert_float(echar)/255.0f;

    expected_int+=vis*1.0; //probability density of uniform distribution is 0.5
    exp_img[imindex]=rgbaFloatToInt(expected_int);
}
#endif
