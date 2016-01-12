//Opencl Pixel conversion functions


//converts an RGB
__kernel void convert_float4_to_rgba( __global float4* exp_img,
                                              __global uint4* imgdims,
                                              __global uint*  gl_im)
{
    uint i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i<(*imgdims).x || j<(*imgdims).y|| i>=(*imgdims).z || j>=(*imgdims).w)
        return;

    float4 intensity = exp_img[imindex];
    gl_im[imindex]   = (rgbaFloatToInt(intensity));//(intensity-*min_i)/range) ;
}
__kernel void convert_float_to_rgba( __global float* exp_img,
                                              __global uint4* imgdims,
                                              __global uint*  gl_im)
{
    uint i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imindex=j*get_global_size(0)+i;
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i<(*imgdims).x || j<(*imgdims).y|| i>=(*imgdims).z || j>=(*imgdims).w)
        return;

    float intensity = exp_img[imindex];
    gl_im[imindex]   = (intensityFloatToInt(intensity));
}
